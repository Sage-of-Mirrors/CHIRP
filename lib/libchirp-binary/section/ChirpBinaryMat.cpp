#include "libchirp-binary/section/ChirpBinaryMat.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>

namespace chirp::binary {
ChirpBinaryMaterials::ChirpBinaryMaterials() : ChirpBinarySection() {}
ChirpBinaryMaterials::ChirpBinaryMaterials(u32 size, u32 elementCount, u16 versionMajor,
                                           u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Materials, size, elementCount, versionMajor, versionMinor) {}
ChirpBinaryMaterials::ChirpBinaryMaterials(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                           u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Materials, offset, size, elementCount, versionMajor,
                     versionMinor) {}

bool ChirpBinaryMaterials::Import(athena::io::IStreamReader& inStream) { return true; }
bool ChirpBinaryMaterials::Export(athena::io::IStreamWriter& outStream) { return true; }

std::unique_ptr< ChirpBinarySection > ChirpBinaryMaterials::CreateMATSection(u32 size,
                                                                             u32 elementCount,
                                                                             u16 versionMajor,
                                                                             u16 versionMinor) {
  return std::make_unique< ChirpBinaryMaterials >(
      ChirpBinaryMaterials(size, elementCount, versionMajor, versionMinor));
}
std::unique_ptr< ChirpBinarySection >
ChirpBinaryMaterials::CreateMATSectionWithOffset(u64 offset, u32 size, u32 elementCount,
                                                 u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinaryMaterials >(
      ChirpBinaryMaterials(offset, size, elementCount, versionMajor, versionMinor));
}
} // namespace chirp::binary
