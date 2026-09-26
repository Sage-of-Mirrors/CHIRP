#pragma once
#include "libchirp-text/ChirpTextDiagnostic.hpp"
#include "libchirp-text/ChirpTextDocument.hpp"
#include "libchirp-text/ChirpTextToken.hpp"
#include <vector>

class ChirpTextParser {
public:
  explicit ChirpTextParser(std::vector< ChirpTextToken > tokens);

  ChirpTextDocument parse(ChirpTextDiagnosticBag& diagnostics);

private:
  const ChirpTextToken& peek(std::size_t offset = 0) const;
  bool check(ChirpTextTokenKind kind) const;
  bool match(ChirpTextTokenKind kind);
  const ChirpTextToken& expect(ChirpTextTokenKind kind, ChirpTextDiagnosticBag& diagnostics,
                               const char* message);
  void fail(ChirpTextDiagnosticBag& diagnostics, const ChirpTextSourceSpan& span,
            const char* message);

  void skipNewlines();
  void parseTopLevel(ChirpTextDocument& document, ChirpTextDiagnosticBag& diagnostics);
  void parseInclude(ChirpTextDocument& document, ChirpTextDiagnosticBag& diagnostics);
  void parseUserData(ChirpTextDocument& document, ChirpTextDiagnosticBag& diagnostics);
  ChirpTextDocument::UserData parseUserDataBlock(ChirpTextDiagnosticBag& diagnostics);
  void parseSection(ChirpTextDocument& document, ChirpTextDiagnosticBag& diagnostics);
  void parseSectionStatement(ChirpTextDocument::Section& section,
                             ChirpTextDiagnosticBag& diagnostics);

  ChirpTextValue parseValue(ChirpTextDiagnosticBag& diagnostics);
  ChirpTextValue parseTuple(ChirpTextDiagnosticBag& diagnostics);
  ChirpTextValue parseScalar(ChirpTextDiagnosticBag& diagnostics);
  ChirpTextDocument::Row parseRow(ChirpTextDiagnosticBag& diagnostics);

  std::vector< ChirpTextToken > mTokens;
  std::size_t mIndex = 0;
  bool mFailed = false;
};
