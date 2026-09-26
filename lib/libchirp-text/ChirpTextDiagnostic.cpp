#include "libchirp-text/ChirpTextDiagnostic.hpp"

#include <algorithm>

void ChirpTextDiagnosticBag::add(const ChirpDiagnosticSeverity severity, ChirpTextSourceSpan span, std::string message) { mDiagnostics.emplace_back(severity, std::move(span), std::move(message)); }

void ChirpTextDiagnosticBag::error(ChirpTextSourceSpan span, std::string message) { add(ChirpDiagnosticSeverity::Error, std::move(span), std::move(message)); }

void ChirpTextDiagnosticBag::warning(ChirpTextSourceSpan span, std::string message) { add(ChirpDiagnosticSeverity::Warning, std::move(span), std::move(message)); }

void ChirpTextDiagnosticBag::note(ChirpTextSourceSpan span, std::string message) { add(ChirpDiagnosticSeverity::Note, std::move(span), std::move(message)); }

bool ChirpTextDiagnosticBag::hasErrors() const {
  return std::ranges::any_of(mDiagnostics, [](const auto& d) { return d.mSeverity == ChirpDiagnosticSeverity::Error; });
}

const std::vector<ChirpDiagnostic>& ChirpTextDiagnosticBag::all() const { return mDiagnostics; }
