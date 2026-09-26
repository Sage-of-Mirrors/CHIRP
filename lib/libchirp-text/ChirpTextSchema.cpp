#include "libchirp-text/ChirpTextSchema.hpp"

#include <format>

namespace chirp::text {
void ChirpTextModelSchema::Validate(const ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics) const {
  if (document.mTypeName != "chirp_model") {
    diagnostics.Error(document.mSpan, "expected document type 'chirp_model'");
    return;
  }

  for (const auto& section : document.mSections) {
    if (section.mCounts.empty()) {
      diagnostics.Warning(section.mSpan, std::format("section '{}' has no count declaration", section.mName));
      continue;
    }

    const auto expected = section.mCounts.front().mValue;
    if (expected < 0) {
      diagnostics.Error(section.mCounts.front().mSpan, "count cannot be negative");
      continue;
    }

    if (static_cast<std::size_t>(expected) != section.mRows.size()) {
      diagnostics.Error(section.mSpan, "declared count does not match number of rows");
    }
  }

  if (const auto* vertices = document.FindSection("vertices")) {
    for (const auto& [fields, span] : vertices->mRows) {
      if (fields.size() != 4)
        diagnostics.Error(span, "vertex rows require four fields");
    }
  }

  if (const auto* surfaces = document.FindSection("surfaces")) {
    for (const auto& [fields, span] : surfaces->mRows) {
      if (fields.size() != 2)
        diagnostics.Error(span, "surface rows require two fields");
    }
  }
}
} // namespace chirp::text