#pragma once
#include "libchirp-text/ChirpTextDiagnostic.hpp"
#include "libchirp-text/ChirpTextToken.hpp"
#include <string>
#include <vector>

class ChirpTextLexer {
public:
  explicit ChirpTextLexer(std::string source, std::string filename = "<memory>");

  std::vector< ChirpTextToken > tokenize(ChirpTextDiagnosticBag& diagnostics);

private:
  char peek(std::size_t offset = 0) const;
  char advance();
  bool eof() const;

  void skipHorizontalWhitespace();
  void lexComment(std::vector< ChirpTextToken >& out);
  void lexString(std::vector< ChirpTextToken >& out, ChirpTextDiagnosticBag& diagnostics);
  void lexNumber(std::vector< ChirpTextToken >& out);
  void lexIdentifier(std::vector< ChirpTextToken >& out);

  std::string mSource;
  std::string mFilename;
  std::size_t mPosition = 0;
  std::size_t mLine = 1;
  std::size_t mColumn = 1;
};
