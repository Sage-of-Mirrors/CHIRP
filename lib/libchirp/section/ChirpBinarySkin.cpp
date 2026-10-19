#include "libchirp/section/ChirpBinarySkin.hpp"

#include <athena/IStreamWriter.hpp>

namespace chirp::binary {
ChirpBinarySkin::ChirpBinarySkin() : ChirpBinarySection() {}
ChirpBinarySkin::ChirpBinarySkin(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Skinning, size, elementCount, versionMajor, versionMinor) {}
ChirpBinarySkin::ChirpBinarySkin(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                 u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Skinning, offset, size, elementCount, versionMajor,
                     versionMinor) {}

bool ChirpBinarySkin::Import(SectionReader& inStream) { return true; }
bool ChirpBinarySkin::Export(athena::io::IStreamWriter& outStream) { return true; }

s64 ChirpBinarySkin::SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset) {
  return 0;
}

std::unique_ptr< ChirpBinarySection >
ChirpBinarySkin::CreateSKNSection(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinarySkin >(
      ChirpBinarySkin(size, elementCount, versionMajor, versionMinor));
}
std::unique_ptr< ChirpBinarySection >
ChirpBinarySkin::CreateSKNSectionWithOffset(u64 offset, u32 size, u32 elementCount,
                                            u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinarySkin >(
      ChirpBinarySkin(offset, size, elementCount, versionMajor, versionMinor));
}
} // namespace chirp::binary