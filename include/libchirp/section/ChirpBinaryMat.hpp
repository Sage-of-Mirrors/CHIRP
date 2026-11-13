#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/material/Material.hpp"
#include "libchirp/section/ChirpBinarySection.hpp"

namespace chirp::gx::material {
class CombinerComponent;
class ColorComponent;
class TextureComponent;
class LightingComponent;
class IndirectTexComponent;
class PixelEngineComponent;
} // namespace chirp::gx::material

namespace chirp::binary {
using ChirpMaterialMap =
    std::unordered_map< std::string, std::unique_ptr< gx::material::Material > >;

class ChirpBinaryMaterials : public ChirpBinarySection {
public:
  ChirpBinaryMaterials();
  ChirpBinaryMaterials(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);
  ChirpBinaryMaterials(u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);

  bool Import(SectionReader& inStream) override;
  bool Export(SectionWriter& outStream) override;

  gx::material::Material* GetMaterial(const std::string& name) const;
  gx::material::Material* CreateMaterial(const std::string& name);

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
    CombinerComponents,
    ColorComponents,
    TextureComponents,
    TextureNames,
    TexCoordGens,
    TexCoordMatrices,
    LightingComponents,
    PixelEngineComponents,
    AlphaCompares,
    BlendModes,
    Fog,
    ZModes,
    IndirectTexComponents,
    StringTable
  };

  void ReadOffsets(SectionReader& sectionReader);
  bool ReadCombinerCompEntries(SectionReader& inStream,
                               gx::material::CombinerComponent& combinerComp, u32 tevStageCount);
  bool ReadColorCompEntries(SectionReader& inStream, gx::material::ColorComponent& colorComp,
                            u32 tevStageCount);
  bool ReadTextureComponent(SectionReader& inStream, gx::material::TextureComponent& texComp);
  bool ReadLightingCompEntries(SectionReader& inStream,
                               gx::material::LightingComponent& lightingComp,
                               u32 lightChannelCount);
  bool ReadIndirectTexComponent(SectionReader& inStream,
                                gx::material::IndirectTexComponent& indirectTexComp);
  bool ReadPixelEngineComponent(SectionReader& inStream,
                                gx::material::PixelEngineComponent& pixelEngineComp);

  u32 m_combinerCompEntriesOffset{0};
  u32 m_colorCompEntriesOffset{0};
  u32 m_textureComponentsOffset{0};
  u32 m_texNamesOffset{0};
  u32 m_texCoordGensOffset{0};
  u32 m_texCoordMatricesOffset{0};
  u32 m_lightingCompEntriesOffset{0};
  u32 m_pixelEngineComponentsOffset{0};
  u32 m_alphaComparesOffset{0};
  u32 m_blendModesOffset{0};
  u32 m_fogOffset{0};
  u32 m_zModesOffset{0};
  u32 m_indirectTexComponentsOffset{0};
  u32 m_stringTableOffset{0};

  ChirpMaterialMap m_materials;
};
} // namespace chirp::binary
