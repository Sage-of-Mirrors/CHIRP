#include "libchirp-text/ChirpTextDocument.hpp"

namespace chirp::text {
const ChirpTextDocument::Section* ChirpTextDocument::FindSection(const std::string& name) const {
  for (const auto& section : mSections) {
    if (section.mName == name) {
      return &section;
    }
  }
  return nullptr;
}

ChirpTextDocument::Section* ChirpTextDocument::FindSection(const std::string& name) {
  for (auto& section : mSections) {
    if (section.mName == name) {
      return &section;
    }
  }
  return nullptr;
}
} // namespace chirp::text
