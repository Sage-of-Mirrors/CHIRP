#include "libchirp-text/ChirpTextLexer.hpp"

#include <cctype>
#include <stdexcept>

namespace chirp::text {
ChirpTextLexer::ChirpTextLexer(std::string source, std::string filename)
: mSource(std::move(source))
, mFilename(std::move(filename)) {}

char ChirpTextLexer::Peek(const std::size_t offset) const { return mPosition + offset < mSource.size() ? mSource[mPosition + offset] : '\0'; }

char ChirpTextLexer::Advance() {
  const char c = Peek();
  if (c == '\0') {
    return c;
  }
  ++mPosition;
  if (c == '\n') {
    ++mLine;
    mColumn = 1;
  } else {
    ++mColumn;
  }
  return c;
}

bool ChirpTextLexer::EndOfFile() const { return mPosition >= mSource.size(); }

void ChirpTextLexer::SkipHorizontalWhitespace() {
  while (!EndOfFile() && (Peek() == ' ' || Peek() == '\t' || Peek() == '\r')) {
    Advance();
  }
}

void ChirpTextLexer::LexComment(std::vector<ChirpTextToken>& out) {
  const ChirpSourceLocation begin{
      .mFile = mFilename,
      .mLine = mLine,
      .mColumn = mColumn,
  };
  Advance();
  std::string text;
  while (!EndOfFile() && Peek() != '\n') {
    text += Advance();
  }

  ChirpTextToken token{
      .mKind = ChirpTextTokenKind::Hash,
      .mText = std::move(text),
      .mSpan =
          {
              .mBegin = begin,
              .mEnd =
                  {
                      .mFile = mFilename,
                      .mLine = mLine,
                      .mColumn = mColumn,
                  },
          },
  };
  out.push_back(std::move(token));
}

void ChirpTextLexer::LexString(std::vector<ChirpTextToken>& out, ChirpDiagnosticBag& diagnostics) {
  const ChirpSourceLocation begin{
      .mFile = mFilename,
      .mLine = mLine,
      .mColumn = mColumn,
  };
  Advance();
  std::string value;
  bool terminated = false;
  while (!EndOfFile()) {
    if (Peek() == '"') {
      Advance();
      terminated = true;
      break;
    }
    if (const char c = Advance(); c == '\\') {
      if (EndOfFile()) {
        break;
      }
      switch (const char escaped = Advance()) {
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
    diagnostics.Error(
        {
            .mBegin = begin,
            .mEnd =
                {
                    .mFile = mFilename,
                    .mLine = mLine,
                    .mColumn = mColumn,
                },
        },
        "unterminated string literal");
    return;
  }

  out.push_back(ChirpTextToken{
      .mKind = ChirpTextTokenKind::String,
      .mText = std::move(value),
      .mSpan =
          {
              .mBegin = begin,
              .mEnd =
                  {
                      .mFile = mFilename,
                      .mLine = mLine,
                      .mColumn = mColumn,
                  },
          },
  });
}

void ChirpTextLexer::LexNumber(std::vector<ChirpTextToken>& out) {
  const ChirpSourceLocation begin{
      .mFile = mFilename,
      .mLine = mLine,
      .mColumn = mColumn,
  };
  std::string text;
  if (Peek() == '-' || Peek() == '+') {
    text += Advance();
  }

  while (std::isdigit(static_cast<unsigned char>(Peek()))) {
    text += Advance();
  }

  bool floating = false;
  if (Peek() == '.') {
    floating = true;
    text += Advance();
    while (std::isdigit(static_cast<unsigned char>(Peek()))) {
      text += Advance();
    }
  }

  if (Peek() == 'e' || Peek() == 'E') {
    floating = true;
    text += Advance();
    if (Peek() == '-' || Peek() == '+') {
      text += Advance();
    }
    while (std::isdigit(static_cast<unsigned char>(Peek()))) {
      text += Advance();
    }
  }

  // Accept the conventional C/C++ floating-point suffix used by Chirp
  // source data, e.g. 0.f and 12.5f. Keep the suffix in the token text;
  // the numeric conversion functions used by the parser accept it.
  if (floating && (Peek() == 'f' || Peek() == 'F')) {
    text += Advance();
  }

  out.push_back(ChirpTextToken{
      .mKind = floating ? ChirpTextTokenKind::Float : ChirpTextTokenKind::Integer,
      .mText = std::move(text),
      .mSpan =
          {
              .mBegin = begin,
              .mEnd =
                  {
                      .mFile = mFilename,
                      .mLine = mLine,
                      .mColumn = mColumn,
                  },
          },
  });
}

void ChirpTextLexer::LexIdentifier(std::vector<ChirpTextToken>& out) {
  const ChirpSourceLocation begin{
      .mFile = mFilename,
      .mLine = mLine,
      .mColumn = mColumn,
  };
  std::string text;
  while (!EndOfFile()) {
    if (const char c = Peek(); std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '.' || c == '-' || c == '$' || c == '/') {
      text += Advance();
    } else {
      break;
    }
  }

  out.push_back(ChirpTextToken{
      .mKind = ChirpTextTokenKind::Identifier,
      .mText = std::move(text),
      .mSpan =
          {
              .mBegin = begin,
              .mEnd =
                  {
                      .mFile = mFilename,
                      .mLine = mLine,
                      .mColumn = mColumn,
                  },
          },
  });
}

std::vector<ChirpTextToken> ChirpTextLexer::tokenize(ChirpDiagnosticBag& diagnostics) {
  std::vector<ChirpTextToken> out;

  while (!EndOfFile()) {
    SkipHorizontalWhitespace();
    if (EndOfFile())
      break;

    const char c = Peek();

    if (c == '\n') {
      const ChirpSourceLocation begin{
          .mFile = mFilename,
          .mLine = mLine,
          .mColumn = mColumn,
      };
      Advance();
      out.push_back({
          .mKind = ChirpTextTokenKind::Newline,
          .mText = "",
          .mSpan =
              {
                  .mBegin = begin,
                  .mEnd =
                      {
                          .mFile = mFilename,
                          .mLine = mLine,
                          .mColumn = mColumn,
                      },
              },
      });
      continue;
    }

    if (c == '#') {
      LexComment(out);
      continue;
    }
    if (c == '"') {
      const auto before = out.size();
      LexString(out, diagnostics);
      if (out.size() == before) {
        break; // fatal lexical error; stop immediately
      }
      continue;
    }
    if (std::isdigit(static_cast<unsigned char>(c)) || ((c == '-' || c == '+') && std::isdigit(static_cast<unsigned char>(Peek(1))))) {
      const auto before = mPosition;
      LexNumber(out);
      if (mPosition == before) {
        break;
      }
      continue;
    }

    const ChirpSourceLocation begin{.mFile = mFilename, .mLine = mLine, .mColumn = mColumn};
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
      if (std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '$') {
        LexIdentifier(out);
        continue;
      }
      diagnostics.Error(
          {
              .mBegin = begin,
              .mEnd =
                  {
                      .mFile = mFilename,
                      .mLine = mLine,
                      .mColumn = mColumn,
                  },
          },
          std::string("unexpected character '") + c + "'");
      const ChirpSourceLocation end{
          .mFile = mFilename,
          .mLine = mLine,
          .mColumn = mColumn,
      };
      out.push_back({
          .mKind = ChirpTextTokenKind::End,
          .mText = "",
          .mSpan =
              {
                  .mBegin = end,
                  .mEnd = end,
              },
      });
      return out;
    }

    Advance();
    out.push_back({
        .mKind = kind,
        .mText = std::string(1, c),
        .mSpan =
            {
                .mBegin = begin,
                .mEnd =
                    {
                        .mFile = mFilename,
                        .mLine = mLine,
                        .mColumn = mColumn,
                    },
            },
    });
  }

  const ChirpSourceLocation end{
      .mFile = mFilename,
      .mLine = mLine,
      .mColumn = mColumn,
  };
  out.push_back({
      .mKind = ChirpTextTokenKind::End,
      .mText = "",
      .mSpan =
          {
              .mBegin = end,
              .mEnd = end,
          },
  });
  return out;
}
} // namespace chirp::text
