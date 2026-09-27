#include "libchirp-binary/ChirpBinarySectionManager.hpp"
#include "libchirp-binary/section/ChirpBinarySection.hpp"

namespace chirp::binary {
ChirpBinarySectionManager& ChirpBinarySectionManager::Instance() {
  static ChirpBinarySectionManager instance;
  return instance;
}

bool ChirpBinarySectionManager::RegisterSection(const FourCC& typeId, FSectionFactory factory) {
  if (mSectionFactories.contains(typeId)) {
    std::cerr << std::format("Factory already registered for {}", typeId.toString()) << std::endl;
    return false;
  }

  mSectionFactories[typeId] = std::move(factory);
  return true;
}

std::unique_ptr<ChirpBinarySection> ChirpBinarySectionManager::NewSection(const FourCC& typeId, const u32 size, const u32 elementCount) {
  if (!mSectionFactories.contains(typeId)) {
    std::cerr << std::format("Factory not registered for {}", typeId.toString()) << std::endl;
    return nullptr;
  }

  return mSectionFactories[typeId](size, elementCount);
}
} // namespace chirp::binary