#pragma once
#include "libchirp-text/ChirpTextSource.hpp"
#include <string>
#include <vector>

enum class ChirpDiagnosticSeverity { Note, Warning, Error };

struct ChirpDiagnostic {
  ChirpDiagnosticSeverity mSeverity;
  ChirpTextSourceSpan mSpan;
  std::string mMessage;
};

class ChirpTextDiagnosticBag {
public:
  void add(ChirpDiagnosticSeverity severity, ChirpTextSourceSpan span, std::string message);
  void error(ChirpTextSourceSpan span, std::string message);
  void warning(ChirpTextSourceSpan span, std::string message);
  void note(ChirpTextSourceSpan span, std::string message);

  [[nodiscard]] bool hasErrors() const;
  [[nodiscard]] const std::vector<ChirpDiagnostic>& all() const;

private:
  std::vector<ChirpDiagnostic> mDiagnostics;
};
