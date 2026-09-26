#include "libchirp-text/ChirpTextLexer.hpp"
#include <cctype>
#include <stdexcept>

ChirpTextLexer::ChirpTextLexer(std::string source, std::string filename)
: mSource(std::move(source)), mFilename(std::move(filename)) {}

char ChirpTextLexer::peek(std::size_t offset) const {
  return mPosition + offset < mSource.size() ? mSource[mPosition + offset] : '\0';
}

char ChirpTextLexer::advance() {
  char c = peek();
  if (c == '\0')
    return c;
  ++mPosition;
  if (c == '\n') {
    ++mLine;
    mColumn = 1;
  } else
    ++mColumn;
  return c;
}

bool ChirpTextLexer::eof() const { return mPosition >= mSource.size(); }

void ChirpTextLexer::skipHorizontalWhitespace() {
  while (!eof() && (peek() == ' ' || peek() == '\t' || peek() == '\r'))
    advance();
}

void ChirpTextLexer::lexComment(std::vector< ChirpTextToken >& out) {
  SourceLocation begin{mFilename, mLine, mColumn};
  advance();
  std::string text;
  while (!eof() && peek() != '\n')
    text += advance();

  ChirpTextToken t;
  t.mKind = ChirpTextTokenKind::Hash;
  t.mText = std::move(text);
  t.mSpan = {begin, {mFilename, mLine, mColumn}};
  out.push_back(std::move(t));
}

void ChirpTextLexer::lexString(std::vector< ChirpTextToken >& out, ChirpTextDiagnosticBag& diagnostics) {
  SourceLocation begin{mFilename, mLine, mColumn};
  advance();
  std::string value;
  bool terminated = false;
  while (!eof()) {
    if (peek() == '"') {
      advance();
      terminated = true;
      break;
    }
    char c = advance();
    if (c == '\\') {
      if (eof())
        break;
      char escaped = advance();
      switch (escaped) {
      case 'n':
        value += '\n';
        break;
      case 'r':
        value += '\r';
        break;
      case 't':
        value += '\t';
        break;
      case '\\':
        value += '\\';
        break;
      case '"':
        value += '"';
        break;
      default:
        value += escaped;
        break;
      }
    } else {
      value += c;
    }
  }

  if (!terminated) {
    diagnostics.error({begin, {mFilename, mLine, mColumn}}, "unterminated string literal");
    return;
  }

  out.push_back(ChirpTextToken{ChirpTextTokenKind::String, std::move(value), {begin, {mFilename, mLine, mColumn}}});
}

void ChirpTextLexer::lexNumber(std::vector< ChirpTextToken >& out) {
  SourceLocation begin{mFilename, mLine, mColumn};
  std::string text;
  if (peek() == '-' || peek() == '+')
    text += advance();

  while (std::isdigit(static_cast< unsigned char >(peek())))
    text += advance();

  bool floating = false;
  if (peek() == '.') {
    floating = true;
    text += advance();
    while (std::isdigit(static_cast< unsigned char >(peek())))
      text += advance();
  }

  if (peek() == 'e' || peek() == 'E') {
    floating = true;
    text += advance();
    if (peek() == '-' || peek() == '+')
      text += advance();
    while (std::isdigit(static_cast< unsigned char >(peek())))
      text += advance();
  }

  // Accept the conventional C/C++ floating-point suffix used by Chirm
  // source data, e.g. 0.f and 12.5f. Keep the suffix in the token text;
  // the numeric conversion functions used by the parser accept it.
  if (floating && (peek() == 'f' || peek() == 'F'))
    text += advance();

  out.push_back(ChirpTextToken{floating ? ChirpTextTokenKind::Float : ChirpTextTokenKind::Integer,
                      std::move(text),
                      {begin, {mFilename, mLine, mColumn}}});
}

void ChirpTextLexer::lexIdentifier(std::vector< ChirpTextToken >& out) {
  SourceLocation begin{mFilename, mLine, mColumn};
  std::string text;
  while (!eof()) {
    char c = peek();
    if (std::isalnum(static_cast< unsigned char >(c)) || c == '_' || c == '.' || c == '-' ||
        c == '$' || c == '/')
      text += advance();
    else
      break;
  }

  out.push_back(
      ChirpTextToken{ChirpTextTokenKind::Identifier, std::move(text), {begin, {mFilename, mLine, mColumn}}});
}

std::vector< ChirpTextToken > ChirpTextLexer::tokenize(ChirpTextDiagnosticBag& diagnostics) {
  std::vector< ChirpTextToken > out;

  while (!eof()) {
    skipHorizontalWhitespace();
    if (eof())
      break;

    char c = peek();

    if (c == '\n') {
      SourceLocation begin{mFilename, mLine, mColumn};
      advance();
      out.push_back({ChirpTextTokenKind::Newline, "", {begin, {mFilename, mLine, mColumn}}});
      continue;
    }

    if (c == '#') {
      lexComment(out);
      continue;
    }
    if (c == '"') {
      const auto before = out.size();
      lexString(out, diagnostics);
      if (out.size() == before)
        break; // fatal lexical error; stop immediately
      continue;
    }
    if (std::isdigit(static_cast< unsigned char >(c)) ||
        ((c == '-' || c == '+') && std::isdigit(static_cast< unsigned char >(peek(1))))) {
      const auto before = mPosition;
      lexNumber(out);
      if (mPosition == before)
        break;
      continue;
    }

    const SourceLocation begin{.mFile = mFilename, .mLine = mLine, .mColumn = mColumn};
    ChirpTextTokenKind kind;
    switch (c) {
    case '@':
      kind = ChirpTextTokenKind::At;
      break;
    case '%':
      kind = ChirpTextTokenKind::Percent;
      break;
    case '(':
      kind = ChirpTextTokenKind::LParen;
      break;
    case ')':
      kind = ChirpTextTokenKind::RParen;
      break;
    case '{':
      kind = ChirpTextTokenKind::LBrace;
      break;
    case '}':
      kind = ChirpTextTokenKind::RBrace;
      break;
    case ',':
      kind = ChirpTextTokenKind::Comma;
      break;
    case '=':
      kind = ChirpTextTokenKind::Equals;
      break;
    default:
      if (std::isalpha(static_cast< unsigned char >(c)) || c == '_' || c == '$') {
        lexIdentifier(out);
        continue;
      }
      diagnostics.error(
          {.mBegin = begin, .mEnd = {.mFile = mFilename, .mLine = mLine, .mColumn = mColumn}},
          std::string("unexpected character '") + c + "'");
      const SourceLocation end{.mFile = mFilename, .mLine = mLine, .mColumn = mColumn};
      out.push_back({.mKind = ChirpTextTokenKind::End, .mText = "", .mSpan = {.mBegin = end, .mEnd = end}});
      return out;
    }

    advance();
    out.push_back({.mKind = kind,
                   .mText = std::string(1, c),
                   .mSpan = {.mBegin = begin,
                             .mEnd = {.mFile = mFilename, .mLine = mLine, .mColumn = mColumn}}});
  }

  const SourceLocation end{.mFile = mFilename, .mLine = mLine, .mColumn = mColumn};
  out.push_back({.mKind = ChirpTextTokenKind::End, .mText = "", .mSpan = {.mBegin = end, .mEnd = end}});
  return out;
}
