#include "libchirp-binary/ChirpBinarySectionManager.hpp"
#include "libchirp-binary/section/ChirpBinarySection.hpp"

namespace chirp::binary {
ChirpBinarySectionManager& ChirpBinarySectionManager::Instance() {
  static ChirpBinarySectionManager instance;
  return instance;
}

bool ChirpBinarySectionManager::RegisterSection(const FourCC& typeId, FSectionFactory factory,
                                                FSectionWithOffsetFactory offsetFactory) {
  if (mSectionFactories.contains(typeId)) {
    std::cerr << std::format("Factory already registered for {}", typeId.toString()) << std::endl;
    return false;
  }

  mSectionFactories[typeId] = {std::move(factory), std::move(offsetFactory)};
  return true;
}

std::unique_ptr< ChirpBinarySection >
ChirpBinarySectionManager::NewSection(const FourCC& typeId, const u32 size, const u32 elementCount,
                                      u16 versionMajor, u16 versionMinor) {
  if (!mSectionFactories.contains(typeId)) {
    std::cerr << std::format("Factory not registered for {}", typeId.toString()) << std::endl;
    return nullptr;
  }

  return mSectionFactories[typeId].first(size, elementCount, versionMajor, versionMinor);
}

std::unique_ptr< ChirpBinarySection >
ChirpBinarySectionManager::NewSectionWithOffset(const FourCC& typeId, const u64 offset,
                                                const u32 size, const u32 elementCount,
                                                u16 versionMajor, u16 versionMinor) {
  if (!mSectionFactories.contains(typeId)) {
    std::cerr << std::format("Factory not registered for {}", typeId.toString()) << std::endl;
    return nullptr;
  }

  return mSectionFactories[typeId].second(offset, size, elementCount, versionMajor, versionMinor);
}
} // namespace chirp::binary