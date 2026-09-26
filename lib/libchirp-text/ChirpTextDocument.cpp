#include "libchirp-text/ChirpTextDocument.hpp"

const ChirpTextDocument::Section* ChirpTextDocument::findSection(const std::string& name) const {
  for (const auto& section : mSections)
    if (section.mName == name)
      return &section;
  return nullptr;
}

ChirpTextDocument::Section* ChirpTextDocument::findSection(const std::string& name) {
  for (auto& section : mSections)
    if (section.mName == name)
      return &section;
  return nullptr;
}
