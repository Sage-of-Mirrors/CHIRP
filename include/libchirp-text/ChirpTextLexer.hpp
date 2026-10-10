#pragma once
#include "libchirp/ChirpDiagnostic.hpp"
#include "libchirp-text/ChirpTextToken.hpp"
#include <string>
#include <vector>

namespace chirp::text {
class ChirpTextLexer {
public:
  explicit ChirpTextLexer(std::string source, std::string filename = "<memory>");

  std::vector<ChirpTextToken> tokenize(ChirpDiagnosticBag& diagnostics);

private:
  char Peek(std::size_t offset = 0) const;
  char Advance();
  bool EndOfFile() const;

  void SkipHorizontalWhitespace();
  void LexComment(std::vector<ChirpTextToken>& out);
  void LexString(std::vector<ChirpTextToken>& out, ChirpDiagnosticBag& diagnostics);
  void LexNumber(std::vector<ChirpTextToken>& out);
  void LexIdentifier(std::vector<ChirpTextToken>& out);

  std::string mSource;
  std::string mFilename;
  std::size_t mPosition = 0;
  std::size_t mLine = 1;
  std::size_t mColumn = 1;
};
} // namespace chirp::text
