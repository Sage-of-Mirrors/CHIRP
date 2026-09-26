#pragma once
#include "libchirp-text/ChirpTextSource.hpp"
#include <string>
#include <vector>

namespace chirp {
enum class ChirpDiagnosticSeverity { Note, Warning, Error };

struct ChirpDiagnostic {
  ChirpDiagnosticSeverity mSeverity;
  ChirpTextSourceSpan mSpan;
  std::string mMessage;
};

class ChirpDiagnosticBag {
public:
  void AddDiagnostic(ChirpDiagnosticSeverity severity, ChirpTextSourceSpan span, std::string message);
  void Error(ChirpTextSourceSpan span, std::string message);
  void Warning(ChirpTextSourceSpan span, std::string message);
  void Note(ChirpTextSourceSpan span, std::string message);

  [[nodiscard]] bool HasErrors() const;
  [[nodiscard]] const std::vector<ChirpDiagnostic>& AllDiagnostics() const;

private:
  std::vector<ChirpDiagnostic> mDiagnostics;
};
} // namespace chirp