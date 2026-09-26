#pragma once
#include "libchirp-text/ChirpTextSource.hpp"
#include <string>
#include <vector>

enum class DiagnosticSeverity { Note, Warning, Error };

struct Diagnostic {
  DiagnosticSeverity mSeverity;
  ChirpTextSourceSpan mSpan;
  std::string message;
};

class ChirpTextDiagnosticBag {
public:
  void add(DiagnosticSeverity severity, ChirpTextSourceSpan span, std::string message);
  void error(ChirpTextSourceSpan span, std::string message);
  void warning(ChirpTextSourceSpan span, std::string message);
  void note(ChirpTextSourceSpan span, std::string message);

  bool hasErrors() const;
  const std::vector< Diagnostic >& all() const;

private:
  std::vector< Diagnostic > mDiagnostics;
};
