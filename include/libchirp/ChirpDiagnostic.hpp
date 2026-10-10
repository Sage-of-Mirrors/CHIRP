#pragma once

#include "libchirp/ChirpErrorSource.hpp"

#include <string>
#include <vector>

namespace chirp {
enum class ChirpDiagnosticSeverity { Note, Warning, Error };

struct ChirpDiagnostic {
  ChirpDiagnosticSeverity mSeverity;
  ChirpSourceSpan mSpan;
  std::string mMessage;
};

class ChirpDiagnosticBag {
public:
  void AddDiagnostic(ChirpDiagnosticSeverity severity, ChirpSourceSpan span, std::string message);
  void Error(ChirpSourceSpan span, std::string message);
  void Warning(ChirpSourceSpan span, std::string message);
  void Note(ChirpSourceSpan span, std::string message);

  [[nodiscard]] bool HasErrors() const;
  [[nodiscard]] const std::vector< ChirpDiagnostic >& AllDiagnostics() const;

private:
  std::vector< ChirpDiagnostic > mDiagnostics;
};
} // namespace chirp