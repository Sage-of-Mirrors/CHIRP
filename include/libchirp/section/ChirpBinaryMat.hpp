#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/Material.hpp"
#include "libchirp/section/ChirpBinarySection.hpp"

namespace chirp::binary {
using ChirpMaterialMap = std::unordered_map< std::string, std::unique_ptr< gx::Material > >;

class ChirpBinaryMaterials : public ChirpBinarySection {
public:
  ChirpBinaryMaterials();
  ChirpBinaryMaterials(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);
  ChirpBinaryMaterials(u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);

  bool Import(SectionReader& inStream) override;
  bool Export(athena::io::IStreamWriter& outStream) override;

  gx::Material* GetMaterial(std::string_view name) const;
  gx::Material* CreateMaterial(std::string_view name, bool colorUpdate = false,
                               bool alphaUpdate = false, bool zCompLoc = false,
                               bool dither = false);

  ChirpMaterialMap& GetMaterials() { return m_materials; }

  static std::unique_ptr< ChirpBinarySection > CreateMATSection(u32 size, u32 elementCount,
                                                                u16 versionMajor, u16 versionMinor);
  static std::unique_ptr< ChirpBinarySection > CreateMATSectionWithOffset(u64 offset, u32 size,
                                                                          u32 elementCount,
                                                                          u16 versionMajor,
                                                                          u16 versionMinor);

protected:
  s64 SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset = 0) override;

private:
  enum class ChunkId : u8 {
    AlphaCompares,
    BlendModes,
    Fog,
    IndirectStages,
    MaterialNames,
    Stages,
    ZModes
  };

  void ReadOffsets(SectionReader& sectionReader);
  void ReadMaterial(SectionReader& sectionReader);

  u32 m_alphaComparesOffset{0};
  u32 m_blendModesOffset{0};
  u32 m_fogOffset{0};
  u32 m_indirectStagesOffset{0};
  u32 m_materialNamesOffset{0};
  u32 m_stagesOffset{0};
  u32 m_zmodesOffset{0};

  ChirpMaterialMap m_materials;
};
} // namespace chirp::binary
