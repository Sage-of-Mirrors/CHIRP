#include "libchirp-text/ChirpTextSchema.hpp"

void ChirpModelSchema::validate(const ChirpTextDocument& document,
                                ChirpTextDiagnosticBag& diagnostics) const {
  if (document.mTypeName != "chirp_model") {
    diagnostics.error(document.mSpan, "expected document type 'chirp_model'");
    return;
  }

  for (const auto& section : document.mSections) {
    if (section.mCounts.empty()) {
      diagnostics.warning(section.mSpan, "section has no count declaration");
      continue;
    }

    const auto expected = section.mCounts.front().mValue;
    if (expected < 0) {
      diagnostics.error(section.mCounts.front().mSpan, "count cannot be negative");
      continue;
    }

    if (static_cast< std::size_t >(expected) != section.mRows.size()) {
      diagnostics.error(section.mSpan, "declared count does not match number of rows");
    }
  }

  if (const auto* vertices = document.findSection("vertices")) {
    for (const auto& row : vertices->mRows) {
      if (row.mFields.size() != 4)
        diagnostics.error(row.mSpan, "vertex rows require four fields");
    }
  }

  if (const auto* surfaces = document.findSection("surfaces")) {
    for (const auto& row : surfaces->mRows) {
      if (row.mFields.size() != 2)
        diagnostics.error(row.mSpan, "surface rows require two fields");
    }
  }
}
