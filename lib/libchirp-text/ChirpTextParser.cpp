#include "libchirp-text/ChirpTextParser.hpp"
#include <cstdlib>

namespace chirp::text {
ChirpTextParser::ChirpTextParser(std::vector<ChirpTextToken> tokens)
: mTokens(std::move(tokens)) {}

const ChirpTextToken& ChirpTextParser::Peek(const std::size_t offset) const {
  const auto i = mIndex + offset < mTokens.size() ? mIndex + offset : mTokens.size() - 1;
  return mTokens[i];
}

bool ChirpTextParser::Check(const ChirpTextTokenKind kind) const { return Peek().mKind == kind; }

bool ChirpTextParser::Match(const ChirpTextTokenKind kind) {
  if (!Check(kind)) {
    return false;
  }
  ++mIndex;
  return true;
}

const ChirpTextToken& ChirpTextParser::Expect(const ChirpTextTokenKind kind, ChirpDiagnosticBag& diagnostics, const char* message) {
  if (!Check(kind)) {
    Fail(diagnostics, Peek().mSpan, message);
    return Peek();
  }
  return mTokens[mIndex++];
}

void ChirpTextParser::Fail(ChirpDiagnosticBag& diagnostics, const ChirpTextSourceSpan& span, const char* message) {
  if (!mFailed) {
    diagnostics.Error(span, message);
    mFailed = true;
  }
}

void ChirpTextParser::SkipNewlines() {
  while (Match(ChirpTextTokenKind::Newline)) {}
}

ChirpTextDocument ChirpTextParser::Parse(ChirpDiagnosticBag& diagnostics) {
  ChirpTextDocument document;
  SkipNewlines();

  if (Check(ChirpTextTokenKind::Identifier)) {
    document.mTypeName = Peek().mText;
    document.mSpan.mBegin = Peek().mSpan.mBegin;
    ++mIndex;
  } else {
    Fail(diagnostics, Peek().mSpan, "expected document type name");
    document.mSpan.mEnd = Peek().mSpan.mEnd;
    return document;
  }

  while (!Check(ChirpTextTokenKind::End)) {
    SkipNewlines();
    if (Check(ChirpTextTokenKind::End))
      break;
    ParseTopLevel(document, diagnostics);
    if (mFailed)
      break;
  }

  document.mSpan.mEnd = Peek().mSpan.mEnd;
  return document;
}

void ChirpTextParser::ParseTopLevel(ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics) {
  if (mFailed)
    return;
  if (Check(ChirpTextTokenKind::At)) {
    if (Peek(1).mKind == ChirpTextTokenKind::Identifier && Peek(1).mText == "include")
      ParseInclude(document, diagnostics);
    else
      ParseUserData(document, diagnostics);
    return;
  }

  if (Check(ChirpTextTokenKind::Percent)) {
    ParseSection(document, diagnostics);
    return;
  }

  if (Check(ChirpTextTokenKind::Hash)) {
    document.mComments.push_back({
        .mText = Peek().mText,
        .mSpan = Peek().mSpan,
    });
    ++mIndex;
    return;
  }

  Fail(diagnostics, Peek().mSpan, "unexpected token at document scope");
}

void ChirpTextParser::ParseInclude(ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics) {
  if (mFailed) {
    return;
  }
  auto at = Expect(ChirpTextTokenKind::At, diagnostics, "expected '@'");
  if (mFailed) {
    return;
  }
  auto keyword = Expect(ChirpTextTokenKind::Identifier, diagnostics, "expected include directive");
  if (mFailed) {
    return;
  }

  if (keyword.mText != "include") {
    diagnostics.Warning(keyword.mSpan, "unknown '@' directive; expected 'include' or namespaced user data");
  }

  Expect(ChirpTextTokenKind::LParen, diagnostics, "expected '(' after include");
  if (mFailed) {
    return;
  }

  const auto& path = Expect(ChirpTextTokenKind::String, diagnostics, "expected include path string");
  if (mFailed) {
    return;
  }
  Expect(ChirpTextTokenKind::RParen, diagnostics, "expected ')' after include path");
  if (mFailed) {
    return;
  }

  ChirpTextDocument::Include include{
      .mPath = path.mText,
      .mSpan =
          {
              .mBegin = at.mSpan.mBegin,
              .mEnd = path.mSpan.mEnd,
          },
  };
  document.mIncludes.push_back(std::move(include));
}

void ChirpTextParser::ParseUserData(ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics) {
  if (mFailed) {
    return;
  }
  ChirpTextDocument::UserData data = ParseUserDataBlock(diagnostics);
  if (mFailed) {
    return;
  }
  document.mUserData.push_back(std::move(data));
}

ChirpTextDocument::UserData ChirpTextParser::ParseUserDataBlock(ChirpDiagnosticBag& diagnostics) {
  ChirpTextDocument::UserData data;
  if (mFailed) {
    return data;
  }

  const auto at = Expect(ChirpTextTokenKind::At, diagnostics, "expected '@'");
  if (mFailed) {
    return data;
  }
  const auto name = Expect(ChirpTextTokenKind::Identifier, diagnostics, "expected user-data namespace");
  if (mFailed) {
    return data;
  }
  SkipNewlines();
  Expect(ChirpTextTokenKind::LBrace, diagnostics, "expected '{' after user-data namespace");
  if (mFailed) {
    return data;
  }

  data.mNamespaceName = name.mText;
  data.mSpan.mBegin = at.mSpan.mBegin;
  SkipNewlines();

  while (!Check(ChirpTextTokenKind::RBrace) && !Check(ChirpTextTokenKind::End)) {
    if (Check(ChirpTextTokenKind::Hash)) {
      ++mIndex;
      SkipNewlines();
      continue;
    }

    // User-data namespaces may contain other user-data namespaces.
    // This keeps the nesting generic: the core parser does not assign
    // any meaning to the namespace names.
    if (Check(ChirpTextTokenKind::At)) {
      data.mChildren.push_back(ParseUserDataBlock(diagnostics));
      if (mFailed) {
        return {};
      }
      SkipNewlines();
      continue;
    }

    const auto key = Expect(ChirpTextTokenKind::Identifier, diagnostics, "expected user-data field name or nested namespace");
    if (mFailed) {
      return data;
    }
    Expect(ChirpTextTokenKind::Equals, diagnostics, "expected '=' after user-data field name");
    if (mFailed) {
      return data;
    }
    ChirpTextValue value = ParseValue(diagnostics);
    if (mFailed) {
      return data;
    }

    data.mFields.push_back(ChirpTextDocument::PropertyStatement{
        .mName = key.mText,
        .mValue = std::move(value),
        .mSpan =
            {
                .mBegin = key.mSpan.mBegin,
                .mEnd = key.mSpan.mEnd,
            },
    });

    SkipNewlines();
  }

  const auto close = Expect(ChirpTextTokenKind::RBrace, diagnostics, "expected '}' after user-data block");
  if (mFailed) {
    return data;
  }
  data.mSpan.mEnd = close.mSpan.mEnd;
  return data;
}

void ChirpTextParser::ParseSection(ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics) {
  if (mFailed) {
    return;
  }
  const auto begin = Expect(ChirpTextTokenKind::Percent, diagnostics, "expected '%'");
  if (mFailed) {
    return;
  }
  const auto name = Expect(ChirpTextTokenKind::Identifier, diagnostics, "expected section name");
  if (mFailed) {
    return;
  }
  Expect(ChirpTextTokenKind::Percent, diagnostics, "expected closing '%' in section header");
  if (mFailed) {
    return;
  }

  ChirpTextDocument::Section section;
  section.mName = name.mText;
  section.mSpan.mBegin = begin.mSpan.mBegin;

  SkipNewlines();

  while (!Check(ChirpTextTokenKind::End)) {
    if (Check(ChirpTextTokenKind::Percent) && Peek(1).mKind == ChirpTextTokenKind::Identifier && Peek(2).mKind == ChirpTextTokenKind::Percent) {
      break;
    }

    if (Check(ChirpTextTokenKind::Hash)) {
      section.mComments.push_back({
          .mText = Peek().mText,
          .mSpan = Peek().mSpan,
      });
      ++mIndex;
      SkipNewlines();
      continue;
    }

    if (Check(ChirpTextTokenKind::Percent) && Peek(1).mKind == ChirpTextTokenKind::LBrace) {
      section.mRows.push_back(ParseRow(diagnostics));
      if (mFailed) {
        return;
      }
      SkipNewlines();
      continue;
    }

    ParseSectionStatement(section, diagnostics);
    if (mFailed) {
      return;
    }
    SkipNewlines();
  }

  section.mSpan.mEnd = Peek().mSpan.mBegin;
  document.mSections.push_back(std::move(section));
}

void ChirpTextParser::ParseSectionStatement(ChirpTextDocument::Section& section, ChirpDiagnosticBag& diagnostics) {
  if (mFailed) {
    return;
  }

  const auto key = Expect(ChirpTextTokenKind::Identifier, diagnostics, "expected section statement");
  if (mFailed) {
    return;
  }
  if (key.mText == "count") {
    if (Match(ChirpTextTokenKind::Equals)) {}
    const auto value = Expect(ChirpTextTokenKind::Integer, diagnostics, "expected integer count");
    if (mFailed) {
      return;
    }
    const ChirpTextDocument::CountStatement count{
        .mValue = std::strtoll(value.mText.c_str(), nullptr, 10),
        .mSpan =
            {
                .mBegin = key.mSpan.mBegin,
                .mEnd = value.mSpan.mEnd,
            },
    };
    section.mCounts.push_back(count);
    return;
  }

  if (Match(ChirpTextTokenKind::Equals)) {
    ChirpTextValue value = ParseValue(diagnostics);
    if (mFailed) {
      return;
    }
    section.mProperties.push_back({
        .mName = key.mText,
        .mValue = std::move(value),
        .mSpan = key.mSpan,
    });
    return;
  }

  Fail(diagnostics, key.mSpan, "expected '=' after section property");
}

ChirpTextValue ChirpTextParser::ParseValue(ChirpDiagnosticBag& diagnostics) {
  if (Check(ChirpTextTokenKind::LParen)) {
    return ParseTuple(diagnostics);
  }
  return ParseScalar(diagnostics);
}

ChirpTextValue ChirpTextParser::ParseTuple(ChirpDiagnosticBag& diagnostics) {
  const auto open = Expect(ChirpTextTokenKind::LParen, diagnostics, "expected '('");
  if (mFailed) {
    return {};
  }

  ChirpTextValue::ChirpTextTuple values;
  SkipNewlines();

  if (!Check(ChirpTextTokenKind::RParen)) {
    while (true) {
      values.push_back(ParseValue(diagnostics));
      if (mFailed) {
        return {};
      }
      if (!Match(ChirpTextTokenKind::Comma)) {
        break;
      }
      SkipNewlines();
    }
  }

  SkipNewlines();
  const auto close = Expect(ChirpTextTokenKind::RParen, diagnostics, "expected ')'");
  if (mFailed) {
    return {};
  }
  return ChirpTextValue{
      std::move(values),
      {
          .mBegin = open.mSpan.mBegin,
          .mEnd = close.mSpan.mEnd,
      },
  };
}

ChirpTextValue ChirpTextParser::ParseScalar(ChirpDiagnosticBag& diagnostics) {
  const auto& token = Peek();

  if (Match(ChirpTextTokenKind::Integer)) {
    return ChirpTextValue{
        static_cast<std::int64_t>(std::strtoll(token.mText.c_str(), nullptr, 10)),
        token.mSpan,
    };
  }

  if (Match(ChirpTextTokenKind::Float)) {
    return ChirpTextValue{
        std::strtod(token.mText.c_str(), nullptr),
        token.mSpan,
    };
  }

  if (Match(ChirpTextTokenKind::String)) {
    return ChirpTextValue{
        token.mText,
        token.mSpan,
    };
  }

  if (Match(ChirpTextTokenKind::Identifier)) {
    if (token.mText == "true") {
      return ChirpTextValue{
          true,
          token.mSpan,
      };
    }
    if (token.mText == "false") {
      return ChirpTextValue{
          false,
          token.mSpan,
      };
    }
    if (token.mText == "null") {
      return ChirpTextValue{
          nullptr,
          token.mSpan,
      };
    }
    return ChirpTextValue{
        token.mText,
        token.mSpan,
    };
  }

  Fail(diagnostics, token.mSpan, "expected value");
  return {};
}

ChirpTextDocument::Row ChirpTextParser::ParseRow(ChirpDiagnosticBag& diagnostics) {
  const auto begin = Expect(ChirpTextTokenKind::Percent, diagnostics, "expected '%{' row opener");
  if (mFailed) {
    return {};
  }
  Expect(ChirpTextTokenKind::LBrace, diagnostics, "expected '{' after '%'");
  if (mFailed) {
    return {};
  }

  ChirpTextDocument::Row row;
  row.mSpan.mBegin = begin.mSpan.mBegin;

  SkipNewlines();
  if (!Check(ChirpTextTokenKind::Percent)) {
    while (true) {
      row.mFields.push_back(ParseValue(diagnostics));
      if (mFailed) {
        return {};
      }
      if (!Match(ChirpTextTokenKind::Comma)) {
        break;
      }
      SkipNewlines();
    }
  }

  SkipNewlines();
  Expect(ChirpTextTokenKind::RBrace, diagnostics, "expected '}' in row");
  if (mFailed) {
    return {};
  }
  const auto end = Expect(ChirpTextTokenKind::Percent, diagnostics, "expected '}%' row terminator");
  if (mFailed) {
    return {};
  }
  row.mSpan.mEnd = end.mSpan.mEnd;
  return row;
}
} // namespace chirp::text
