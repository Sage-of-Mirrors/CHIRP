#include "libchirp-text/ChirpTextDiagnostic.hpp"

void ChirpTextDiagnosticBag::add(DiagnosticSeverity severity, ChirpTextSourceSpan span,
                                 std::string message) {
  mDiagnostics.push_back({severity, std::move(span), std::move(message)});
}

void ChirpTextDiagnosticBag::error(ChirpTextSourceSpan span, std::string message) {
  add(DiagnosticSeverity::Error, std::move(span), std::move(message));
}

void ChirpTextDiagnosticBag::warning(ChirpTextSourceSpan span, std::string message) {
  add(DiagnosticSeverity::Warning, std::move(span), std::move(message));
}

void ChirpTextDiagnosticBag::note(ChirpTextSourceSpan span, std::string message) {
  add(DiagnosticSeverity::Note, std::move(span), std::move(message));
}

bool ChirpTextDiagnosticBag::hasErrors() const {
  for (const auto& d : mDiagnostics)
    if (d.mSeverity == DiagnosticSeverity::Error)
      return true;
  return false;
}

const std::vector< Diagnostic >& ChirpTextDiagnosticBag::all() const { return mDiagnostics; }
