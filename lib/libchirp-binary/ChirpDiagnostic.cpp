#include "libchirp-binary/ChirpDiagnostic.hpp"

#include <algorithm>

namespace chirp {
void ChirpDiagnosticBag::AddDiagnostic(const ChirpDiagnosticSeverity severity, ChirpSourceSpan span, std::string message) {
  mDiagnostics.emplace_back(severity, std::move(span), std::move(message));
}

void ChirpDiagnosticBag::Error(ChirpSourceSpan span, std::string message) { AddDiagnostic(ChirpDiagnosticSeverity::Error, std::move(span), std::move(message)); }

void ChirpDiagnosticBag::Warning(ChirpSourceSpan span, std::string message) { AddDiagnostic(ChirpDiagnosticSeverity::Warning, std::move(span), std::move(message)); }

void ChirpDiagnosticBag::Note(ChirpSourceSpan span, std::string message) { AddDiagnostic(ChirpDiagnosticSeverity::Note, std::move(span), std::move(message)); }

bool ChirpDiagnosticBag::HasErrors() const {
  return std::ranges::any_of(mDiagnostics, [](const auto& d) { return d.mSeverity == ChirpDiagnosticSeverity::Error; });
}

const std::vector<ChirpDiagnostic>& ChirpDiagnosticBag::AllDiagnostics() const { return mDiagnostics; }
} // namespace chirp
