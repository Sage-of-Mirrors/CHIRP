#pragma once
#include "libchirp-common/ChirpDiagnostic.hpp"
#include "libchirp-text/ChirpTextDocument.hpp"
#include "libchirp-text/ChirpTextToken.hpp"
#include <vector>

namespace chirp::text {
class ChirpTextParser {
public:
  explicit ChirpTextParser(std::vector<ChirpTextToken> tokens);

  ChirpTextDocument Parse(ChirpDiagnosticBag& diagnostics);

private:
  [[nodiscard]] const ChirpTextToken& Peek(std::size_t offset = 0) const;
  [[nodiscard]] bool Check(ChirpTextTokenKind kind) const;
  bool Match(ChirpTextTokenKind kind);
  const ChirpTextToken& Expect(ChirpTextTokenKind kind, ChirpDiagnosticBag& diagnostics, const char* message);
  void Fail(ChirpDiagnosticBag& diagnostics, const ChirpSourceSpan& span, const char* message);

  void SkipNewlines();
  void ParseTopLevel(ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics);
  void ParseInclude(ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics);
  void ParseUserData(ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics);
  [[nodiscard]] ChirpTextDocument::UserData ParseUserDataBlock(ChirpDiagnosticBag& diagnostics);
  void ParseSection(ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics);
  void ParseSectionStatement(ChirpTextDocument::Section& section, ChirpDiagnosticBag& diagnostics);

  [[nodiscard]] ChirpTextValue ParseValue(ChirpDiagnosticBag& diagnostics);
  [[nodiscard]] ChirpTextValue ParseTuple(ChirpDiagnosticBag& diagnostics);
  [[nodiscard]] ChirpTextValue ParseScalar(ChirpDiagnosticBag& diagnostics);
  [[nodiscard]] ChirpTextDocument::Row ParseRow(ChirpDiagnosticBag& diagnostics);
  std::vector<ChirpTextToken> mTokens;
  std::size_t mIndex = 0;
  bool mFailed = false;
};
} // namespace chirp::text