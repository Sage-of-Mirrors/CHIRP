#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/Material.hpp"
#include "libchirp/section/ChirpBinarySection.hpp"

namespace chirp::binary {
class ChirpBinaryMaterials : public ChirpBinarySection {
public:
  ChirpBinaryMaterials();
  ChirpBinaryMaterials(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);
  ChirpBinaryMaterials(u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);

  bool Import(SectionReader& inStream) override;
  bool Export(athena::io::IStreamWriter& outStream) override;

  static std::unique_ptr< ChirpBinarySection > CreateMATSection(u32 size, u32 elementCount,
                                                                u16 versionMajor, u16 versionMinor);
  static std::unique_ptr< ChirpBinarySection > CreateMATSectionWithOffset(u64 offset, u32 size,
                                                                          u32 elementCount,
                                                                          u16 versionMajor,
                                                                          u16 versionMinor);

protected:
  s64 SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset = 0) override;

private:
  enum class ChunkId : u8 { AlphaCompares, BlendModes, Fog };

  u32 m_alphaComparesOffset{0};
  u32 m_blendModesOffset{0};
  u32 m_fogOffset{0};
};
} // namespace chirp::binary
