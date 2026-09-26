#include "libchirp-text/ChirpTextParser.hpp"
#include <cstdlib>
#include <stdexcept>

ChirpTextParser::ChirpTextParser(std::vector< ChirpTextToken > tokens) : mTokens(std::move(tokens)) {}

const ChirpTextToken& ChirpTextParser::peek(std::size_t offset) const {
  const auto i = mIndex + offset < mTokens.size() ? mIndex + offset : mTokens.size() - 1;
  return mTokens[i];
}

bool ChirpTextParser::check(ChirpTextTokenKind kind) const { return peek().mKind == kind; }

bool ChirpTextParser::match(ChirpTextTokenKind kind) {
  if (!check(kind))
    return false;
  ++mIndex;
  return true;
}

const ChirpTextToken& ChirpTextParser::expect(ChirpTextTokenKind kind, ChirpTextDiagnosticBag& diagnostics,
                                     const char* message) {
  if (!check(kind)) {
    fail(diagnostics, peek().mSpan, message);
    return peek();
  }
  return mTokens[mIndex++];
}

void ChirpTextParser::fail(ChirpTextDiagnosticBag& diagnostics, const ChirpTextSourceSpan& span,
                           const char* message) {
  if (!mFailed) {
    diagnostics.error(span, message);
    mFailed = true;
  }
}

void ChirpTextParser::skipNewlines() {
  while (match(ChirpTextTokenKind::Newline)) {
  }
}

ChirpTextDocument ChirpTextParser::parse(ChirpTextDiagnosticBag& diagnostics) {
  ChirpTextDocument document;
  skipNewlines();

  if (check(ChirpTextTokenKind::Identifier)) {
    document.mTypeName = peek().mText;
    document.mSpan.mBegin = peek().mSpan.mBegin;
    ++mIndex;
  } else {
    fail(diagnostics, peek().mSpan, "expected document type name");
    document.mSpan.mEnd = peek().mSpan.mEnd;
    return document;
  }

  while (!check(ChirpTextTokenKind::End)) {
    skipNewlines();
    if (check(ChirpTextTokenKind::End))
      break;
    parseTopLevel(document, diagnostics);
    if (mFailed)
      break;
  }

  document.mSpan.mEnd = peek().mSpan.mEnd;
  return document;
}

void ChirpTextParser::parseTopLevel(ChirpTextDocument& document,
                                    ChirpTextDiagnosticBag& diagnostics) {
  if (mFailed)
    return;
  if (check(ChirpTextTokenKind::At)) {
    if (peek(1).mKind == ChirpTextTokenKind::Identifier && peek(1).mText == "include")
      parseInclude(document, diagnostics);
    else
      parseUserData(document, diagnostics);
    return;
  }

  if (check(ChirpTextTokenKind::Percent)) {
    parseSection(document, diagnostics);
    return;
  }

  if (check(ChirpTextTokenKind::Hash)) {
    document.mComments.push_back({peek().mText, peek().mSpan});
    ++mIndex;
    return;
  }

  fail(diagnostics, peek().mSpan, "unexpected token at document scope");
}

void ChirpTextParser::parseInclude(ChirpTextDocument& document,
                                   ChirpTextDiagnosticBag& diagnostics) {
  if (mFailed)
    return;
  auto at = expect(ChirpTextTokenKind::At, diagnostics, "expected '@'");
  if (mFailed)
    return;
  auto keyword = expect(ChirpTextTokenKind::Identifier, diagnostics, "expected include directive");
  if (mFailed)
    return;

  if (keyword.mText != "include")
    diagnostics.warning(keyword.mSpan,
                        "unknown '@' directive; expected 'include' or namespaced user data");

  expect(ChirpTextTokenKind::LParen, diagnostics, "expected '(' after include");
  if (mFailed)
    return;

  const auto& path = expect(ChirpTextTokenKind::String, diagnostics, "expected include path string");
  if (mFailed)
    return;
  expect(ChirpTextTokenKind::RParen, diagnostics, "expected ')' after include path");
  if (mFailed)
    return;

  ChirpTextDocument::Include include;
  include.mPath = path.mText;
  include.mSpan = {at.mSpan.mBegin, path.mSpan.mEnd};
  document.mIncludes.push_back(std::move(include));
}

void ChirpTextParser::parseUserData(ChirpTextDocument& document,
                                    ChirpTextDiagnosticBag& diagnostics) {
  if (mFailed)
    return;
  ChirpTextDocument::UserData data = parseUserDataBlock(diagnostics);
  if (mFailed)
    return;
  document.mUserData.push_back(std::move(data));
}

ChirpTextDocument::UserData
ChirpTextParser::parseUserDataBlock(ChirpTextDiagnosticBag& diagnostics) {
  ChirpTextDocument::UserData data;
  if (mFailed)
    return data;

  const auto at = expect(ChirpTextTokenKind::At, diagnostics, "expected '@'");
  if (mFailed)
    return data;
  const auto name = expect(ChirpTextTokenKind::Identifier, diagnostics, "expected user-data namespace");
  if (mFailed)
    return data;
  skipNewlines();
  expect(ChirpTextTokenKind::LBrace, diagnostics, "expected '{' after user-data namespace");
  if (mFailed)
    return data;

  data.mNamespaceName = name.mText;
  data.mSpan.mBegin = at.mSpan.mBegin;
  skipNewlines();

  while (!check(ChirpTextTokenKind::RBrace) && !check(ChirpTextTokenKind::End)) {
    if (check(ChirpTextTokenKind::Hash)) {
      ++mIndex;
      skipNewlines();
      continue;
    }

    // User-data namespaces may contain other user-data namespaces.
    // This keeps the nesting generic: the core parser does not assign
    // any meaning to the namespace names.
    if (check(ChirpTextTokenKind::At)) {
      data.mChildren.push_back(parseUserDataBlock(diagnostics));
      if (mFailed)
        return {};
      skipNewlines();
      continue;
    }

    const auto key = expect(ChirpTextTokenKind::Identifier, diagnostics,
                            "expected user-data field name or nested namespace");
    if (mFailed)
      return data;
    expect(ChirpTextTokenKind::Equals, diagnostics, "expected '=' after user-data field name");
    if (mFailed)
      return data;
    ChirpTextValue value = parseValue(diagnostics);
    if (mFailed)
      return data;

    data.mFields.push_back(
        ChirpTextDocument::PropertyStatement{key.mText, std::move(value), {key.mSpan.mBegin, key.mSpan.mEnd}});

    skipNewlines();
  }

  const auto close = expect(ChirpTextTokenKind::RBrace, diagnostics, "expected '}' after user-data block");
  if (mFailed)
    return data;
  data.mSpan.mEnd = close.mSpan.mEnd;
  return data;
}

void ChirpTextParser::parseSection(ChirpTextDocument& document, ChirpTextDiagnosticBag& diagnostics) {
  if (mFailed)
    return;
  const auto begin = expect(ChirpTextTokenKind::Percent, diagnostics, "expected '%'");
  if (mFailed)
    return;
  const auto name = expect(ChirpTextTokenKind::Identifier, diagnostics, "expected section name");
  if (mFailed)
    return;
  expect(ChirpTextTokenKind::Percent, diagnostics, "expected closing '%' in section header");
  if (mFailed)
    return;

  ChirpTextDocument::Section section;
  section.mName = name.mText;
  section.mSpan.mBegin = begin.mSpan.mBegin;

  skipNewlines();

  while (!check(ChirpTextTokenKind::End)) {
    if (check(ChirpTextTokenKind::Percent) && peek(1).mKind == ChirpTextTokenKind::Identifier &&
        peek(2).mKind == ChirpTextTokenKind::Percent) {
      break;
    }

    if (check(ChirpTextTokenKind::Hash)) {
      section.mComments.push_back({peek().mText, peek().mSpan});
      ++mIndex;
      skipNewlines();
      continue;
    }

    if (check(ChirpTextTokenKind::Percent) && peek(1).mKind == ChirpTextTokenKind::LBrace) {
      section.mRows.push_back(parseRow(diagnostics));
      if (mFailed)
        return;
      skipNewlines();
      continue;
    }

    parseSectionStatement(section, diagnostics);
    if (mFailed)
      return;
    skipNewlines();
  }

  section.mSpan.mEnd = peek().mSpan.mBegin;
  document.mSections.push_back(std::move(section));
}

void ChirpTextParser::parseSectionStatement(ChirpTextDocument::Section& section,
                                            ChirpTextDiagnosticBag& diagnostics) {
  if (mFailed)
    return;
  const auto key = expect(ChirpTextTokenKind::Identifier, diagnostics, "expected section statement");
  if (mFailed)
    return;
  if (key.mText == "count") {
    if (match(ChirpTextTokenKind::Equals)) {
    }
    const auto value = expect(ChirpTextTokenKind::Integer, diagnostics, "expected integer count");
    if (mFailed)
      return;
    ChirpTextDocument::CountStatement count;
    count.mValue = std::strtoll(value.mText.c_str(), nullptr, 10);
    count.mSpan = {key.mSpan.mBegin, value.mSpan.mEnd};
    section.mCounts.push_back(count);
    return;
  }

  if (match(ChirpTextTokenKind::Equals)) {
    ChirpTextValue value = parseValue(diagnostics);
    if (mFailed)
      return;
    section.mProperties.push_back({key.mText, std::move(value), key.mSpan});
    return;
  }

  fail(diagnostics, key.mSpan, "expected '=' after section property");
}

ChirpTextValue ChirpTextParser::parseValue(ChirpTextDiagnosticBag& diagnostics) {
  if (check(ChirpTextTokenKind::LParen))
    return parseTuple(diagnostics);
  return parseScalar(diagnostics);
}

ChirpTextValue ChirpTextParser::parseTuple(ChirpTextDiagnosticBag& diagnostics) {
  const auto open = expect(ChirpTextTokenKind::LParen, diagnostics, "expected '('");
  if (mFailed)
    return {};
  ChirpTextValue::ChirpTextTuple values;
  skipNewlines();

  if (!check(ChirpTextTokenKind::RParen)) {
    while (true) {
      values.push_back(parseValue(diagnostics));
      if (mFailed)
        return {};
      if (!match(ChirpTextTokenKind::Comma))
        break;
      skipNewlines();
    }
  }

  skipNewlines();
  const auto close = expect(ChirpTextTokenKind::RParen, diagnostics, "expected ')'");
  if (mFailed)
    return {};
  return ChirpTextValue{std::move(values), {open.mSpan.mBegin, close.mSpan.mEnd}};
}

ChirpTextValue ChirpTextParser::parseScalar(ChirpTextDiagnosticBag& diagnostics) {
  const auto& token = peek();

  if (match(ChirpTextTokenKind::Integer))
    return ChirpTextValue{static_cast< std::int64_t >(std::strtoll(token.mText.c_str(), nullptr, 10)),
                 token.mSpan};

  if (match(ChirpTextTokenKind::Float))
    return ChirpTextValue{std::strtod(token.mText.c_str(), nullptr), token.mSpan};

  if (match(ChirpTextTokenKind::String))
    return ChirpTextValue{token.mText, token.mSpan};

  if (match(ChirpTextTokenKind::Identifier)) {
    if (token.mText == "true")
      return ChirpTextValue{true, token.mSpan};
    if (token.mText == "false")
      return ChirpTextValue{false, token.mSpan};
    if (token.mText == "null")
      return ChirpTextValue{nullptr, token.mSpan};
    return ChirpTextValue{token.mText, token.mSpan};
  }

  fail(diagnostics, token.mSpan, "expected value");
  return {};
}

ChirpTextDocument::Row ChirpTextParser::parseRow(ChirpTextDiagnosticBag& diagnostics) {
  const auto begin = expect(ChirpTextTokenKind::Percent, diagnostics, "expected '%{' row opener");
  if (mFailed)
    return {};
  expect(ChirpTextTokenKind::LBrace, diagnostics, "expected '{' after '%'");
  if (mFailed)
    return {};

  ChirpTextDocument::Row row;
  row.mSpan.mBegin = begin.mSpan.mBegin;

  skipNewlines();
  if (!check(ChirpTextTokenKind::Percent)) {
    while (true) {
      row.mFields.push_back(parseValue(diagnostics));
      if (mFailed)
        return {};
      if (!match(ChirpTextTokenKind::Comma))
        break;
      skipNewlines();
    }
  }

  skipNewlines();
  expect(ChirpTextTokenKind::RBrace, diagnostics, "expected '}' in row");
  if (mFailed)
    return {};
  const auto end = expect(ChirpTextTokenKind::Percent, diagnostics, "expected '%}' row terminator");
  if (mFailed)
    return {};
  row.mSpan.mEnd = end.mSpan.mEnd;
  return row;
}
