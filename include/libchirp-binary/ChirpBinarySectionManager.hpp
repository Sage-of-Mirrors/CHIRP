#pragma once

#include "libchirp-binary/ChirpCommon.hpp"

#include <functional>
#include <map>

namespace chirp::binary {
class ChirpBinarySection;
class ChirpBinarySectionManager {
public:
  using FSectionFactory = std::function< std::unique_ptr< ChirpBinarySection >(
      u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor) >;
  using FSectionWithOffsetFactory = std::function< std::unique_ptr< ChirpBinarySection >(
      u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor) >;

  ChirpBinarySectionManager(const ChirpBinarySectionManager&) = delete;
  ChirpBinarySectionManager& operator=(const ChirpBinarySectionManager&) = delete;
  ChirpBinarySectionManager(const ChirpBinarySectionManager&&) = delete;
  ChirpBinarySectionManager& operator=(const ChirpBinarySectionManager&&) = delete;

  static ChirpBinarySectionManager& Instance();

  bool RegisterSection(const FourCC& typeId, FSectionFactory factory,
                       FSectionWithOffsetFactory offsetFactory);

  std::unique_ptr< ChirpBinarySection > NewSection(const FourCC& typeId, u32 size, u32 elementCount,
                                                   u16 versionMajor = 0, u16 versionMinor = 0);

  std::unique_ptr< ChirpBinarySection > NewSectionWithOffset(const FourCC& typeId, u64 offset,
                                                             u32 size, u32 elementCount,
                                                             u16 versionMajor = 0,
                                                             u16 versionMinor = 0);

private:
  ChirpBinarySectionManager() = default;

  std::map< FourCC, std::pair< FSectionFactory, FSectionWithOffsetFactory > > mSectionFactories;
};
} // namespace chirp::binary