#include "libchirp/section/ChirpBinaryMat.hpp"
#include "libchirp/math/ChirpColor.hpp"
#include "libchirp/math/ChirpColorS10.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>

namespace {
struct MaterialBinaryEntry {
  /*0x00*/ u16 m_nameSize{0};
  /*0x02*/ u16 m_tevStageCount{0};
  /*0x04*/ u16 m_lightChannelCount{0};
  /*0x06*/ u16 m_pad0{0};

  /*0x08*/ u32 m_nameOffset{0};
  /*0x0C*/ u32 m_firstCombinerCompEntryOffset{0};
  /*0x10*/ u32 m_firstColorCompEntryOffset{0};
  /*0x14*/ u32 m_textureComponentOffset{0};
  /*0x18*/ u32 m_firstLightingCompEntryOffset{0};
  /*0x1C*/ u32 m_indirectTexComponentOffset{0};
  /*0x20*/ u32 m_pixelEngineComponentOffset{0};
};

MaterialBinaryEntry ReadMaterialData(chirp::binary::SectionReader& sectionReader) {
  MaterialBinaryEntry matData{};
  matData.m_nameSize = sectionReader.readUint16();
  matData.m_tevStageCount = sectionReader.readUint16();
  matData.m_lightChannelCount = sectionReader.readUint16();
  sectionReader.seekAlign4();

  matData.m_nameOffset = sectionReader.readUint32();
  matData.m_firstCombinerCompEntryOffset = sectionReader.readUint32();
  matData.m_firstColorCompEntryOffset = sectionReader.readUint32();
  matData.m_textureComponentOffset = sectionReader.readUint32();
  matData.m_firstLightingCompEntryOffset = sectionReader.readUint32();
  matData.m_indirectTexComponentOffset = sectionReader.readUint32();
  matData.m_pixelEngineComponentOffset = sectionReader.readUint32();

  return matData;
}
} // namespace

namespace chirp::binary {
ChirpBinaryMaterials::ChirpBinaryMaterials() : ChirpBinarySection() {}
ChirpBinaryMaterials::ChirpBinaryMaterials(u32 size, u32 elementCount, u16 versionMajor,
                                           u16 versionMinor)
: ChirpBinarySection(size, elementCount, versionMajor, versionMinor) {}
ChirpBinaryMaterials::ChirpBinaryMaterials(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                           u16 versionMinor)
: ChirpBinarySection(offset, size, elementCount, versionMajor, versionMinor) {}

bool ChirpBinaryMaterials::Import(SectionReader& inStream) {
  ReadOffsets(inStream);
  inStream.seekAlign16();

  for (u32 i = 0; i < m_elementCount; i++) {
    MaterialBinaryEntry matData = ReadMaterialData(inStream);

    auto nextMatPos =
        SeekToChunk(inStream, static_cast< u8 >(ChunkId::StringTable), matData.m_nameOffset);
    std::string newMatName = inStream.readString(matData.m_nameSize);

    gx::material::Material* newMaterial = CreateMaterial(newMatName);
    if (!newMaterial) {
      std::cout << "Failed to create material named \"" << newMatName << "\"." << std::endl;
      return false;
    }

    // Read in the combiner component.
    SeekToChunk(inStream, static_cast< u8 >(ChunkId::CombinerComponents),
                matData.m_firstCombinerCompEntryOffset);
    if (!ReadCombinerCompEntries(inStream, newMaterial->GetCombinerComponent(),
                                 matData.m_tevStageCount)) {
      std::cout << "Failed to read combiner component for material named \"" << newMatName << "\"."
                << std::endl;
      return false;
    }

    // Read in the color component.
    SeekToChunk(inStream, static_cast< u8 >(ChunkId::ColorComponents),
                matData.m_firstColorCompEntryOffset);
    ReadColorCompEntries(inStream, newMaterial->GetColorComponent(), matData.m_tevStageCount);

    // Read in the texture component.
    SeekToChunk(inStream, static_cast< u8 >(ChunkId::TextureComponents),
                matData.m_textureComponentOffset);
    ReadTextureComponent(inStream, newMaterial->GetTextureComponent());

    // Read in the lighting component.
    SeekToChunk(inStream, static_cast< u8 >(ChunkId::LightingComponents),
                matData.m_firstLightingCompEntryOffset);
    ReadLightingCompEntries(inStream, newMaterial->GetLightingComponent(),
                            matData.m_lightChannelCount);

    // Read in indirect texturing component.
    SeekToChunk(inStream, static_cast< u8 >(ChunkId::IndirectTexComponents),
                matData.m_indirectTexComponentOffset);
    ReadIndirectTexComponent(inStream, newMaterial->GetIndirectTexComponent());

    // Read in pixel engine component.
    SeekToChunk(inStream, static_cast< u8 >(ChunkId::PixelEngineComponents),
                matData.m_pixelEngineComponentOffset);
    ReadPixelEngineComponent(inStream, newMaterial->GetPixelEngineComponent());

    // Seek back to the start of the next material entry.
    inStream.seek(nextMatPos, athena::SeekOrigin::Begin);
  }

  return true;
}

bool ChirpBinaryMaterials::Export(SectionWriter& outStream) { return true; }

gx::material::Material* ChirpBinaryMaterials::GetMaterial(const std::string& name) const {
  if (auto itr = m_materials.find(name); itr != m_materials.end()) {
    return itr->second.get();
  }

  std::cout << "Attempted to get a material named \"" << name
            << "\" from a ChirpBinaryMaterials that did not have a material with that name."
            << std::endl;
  return nullptr;
}

gx::material::Material* ChirpBinaryMaterials::CreateMaterial(const std::string& name) {
  auto itr = m_materials.try_emplace(name, std::make_unique< gx::material::Material >(name));
  if (!itr.second) {
    std::cout << "Attempted to add a material to ChirpBinaryMaterials named \"" << name
              << "\", but a material with that name already existed." << std::endl;
    return nullptr;
  }

  return itr.first->second.get();
}

void ChirpBinaryMaterials::ReadOffsets(SectionReader& inStream) {
  m_combinerCompEntriesOffset = inStream.readUint32();
  m_colorCompEntriesOffset = inStream.readUint32();
  m_textureComponentsOffset = inStream.readUint32();
  m_texNamesOffset = inStream.readUint32();
  m_texCoordGensOffset = inStream.readUint32();
  m_texCoordMatricesOffset = inStream.readUint32();
  m_lightingCompEntriesOffset = inStream.readUint32();
  m_pixelEngineComponentsOffset = inStream.readUint32();
  m_alphaComparesOffset = inStream.readUint32();
  m_blendModesOffset = inStream.readUint32();
  m_fogOffset = inStream.readUint32();
  m_zModesOffset = inStream.readUint32();
  m_indirectTexComponentsOffset = inStream.readUint32();
  m_stringTableOffset = inStream.readUint32();
}

bool ChirpBinaryMaterials::ReadCombinerCompEntries(SectionReader& inStream,
                                                   gx::material::CombinerComponent& combinerComp,
                                                   u32 tevStageCount) {
  if (tevStageCount == 0 || tevStageCount > gx::material::kMaxTevStageCount) {
    return false;
  }

  for (u32 i = 0; i < tevStageCount; i++) {
    gx::material::TevStage* newStage = combinerComp.AddTevStage();
    if (!newStage) {
      return false;
    }

    // Read color combiner.
    newStage->m_colorCombiner.m_colorArgA = static_cast< gx::TevColorArg >(inStream.readUByte());
    newStage->m_colorCombiner.m_colorArgB = static_cast< gx::TevColorArg >(inStream.readUByte());
    newStage->m_colorCombiner.m_colorArgC = static_cast< gx::TevColorArg >(inStream.readUByte());
    newStage->m_colorCombiner.m_colorArgD = static_cast< gx::TevColorArg >(inStream.readUByte());
    newStage->m_colorCombiner.m_colorOp = static_cast< gx::TevOp >(inStream.readUByte());
    newStage->m_colorCombiner.m_colorBias = static_cast< gx::TevBias >(inStream.readUByte());
    newStage->m_colorCombiner.m_colorScale = static_cast< gx::TevScale >(inStream.readUByte());
    newStage->m_colorCombiner.m_colorClamp = inStream.readBool();
    newStage->m_colorCombiner.m_colorOutReg = static_cast< gx::TevRegId >(inStream.readUByte());

    // Read alpha combiner.
    newStage->m_alphaCombiner.m_alphaArgA = static_cast< gx::TevAlphaArg >(inStream.readUByte());
    newStage->m_alphaCombiner.m_alphaArgB = static_cast< gx::TevAlphaArg >(inStream.readUByte());
    newStage->m_alphaCombiner.m_alphaArgC = static_cast< gx::TevAlphaArg >(inStream.readUByte());
    newStage->m_alphaCombiner.m_alphaArgD = static_cast< gx::TevAlphaArg >(inStream.readUByte());
    newStage->m_alphaCombiner.m_alphaOp = static_cast< gx::TevOp >(inStream.readUByte());
    newStage->m_alphaCombiner.m_alphaBias = static_cast< gx::TevBias >(inStream.readUByte());
    newStage->m_alphaCombiner.m_alphaScale = static_cast< gx::TevScale >(inStream.readUByte());
    newStage->m_alphaCombiner.m_alphaClamp = inStream.readBool();
    newStage->m_alphaCombiner.m_alphaOutReg = static_cast< gx::TevRegId >(inStream.readUByte());

    // Read TEV order.
    newStage->m_tevOrder.m_texCoordId = static_cast< gx::TexCoordId >(inStream.readUByte());
    newStage->m_tevOrder.m_texMapId = static_cast< gx::TexMapId >(inStream.readUByte());
    newStage->m_tevOrder.m_channelId = static_cast< gx::ChannelId >(inStream.readUByte());

    // Skip 4 padding bytes.
    inStream.seekAlign4();
  }

  return true;
}

bool ChirpBinaryMaterials::ReadColorCompEntries(SectionReader& inStream,
                                                gx::material::ColorComponent& colorComp,
                                                u32 tevStageCount) {
  if (tevStageCount == 0 || tevStageCount > gx::material::kMaxTevStageCount) {
    return false;
  }

  for (u32 i = 0; i < tevStageCount; i++) {
    // Read konst sels.
    *colorComp.GetTevKonstColorSel(i) = static_cast< gx::TevKColorSel >(inStream.readUByte());
    *colorComp.GetTevKonstAlphaSel(i) = static_cast< gx::TevKAlphaSel >(inStream.readUByte());

    // Read swap sels.
    *colorComp.GetTevKonstSwapSel(i) = static_cast< gx::TevSwapSel >(inStream.readUByte());
    *colorComp.GetTevTexSwapSel(i) = static_cast< gx::TevSwapSel >(inStream.readUByte());

    // Read reg colors.
    for (u32 regColorIdx = 0; regColorIdx < static_cast< u32 >(gx::TevRegId::Max); regColorIdx++) {
      *colorComp.GetTevRegColor(i, static_cast< gx::TevRegId >(regColorIdx)) = math::ChirpColorS10{
          inStream.readInt16(), inStream.readInt16(), inStream.readInt16(), inStream.readInt16()};
    }

    // Read konst colors.
    for (u32 konstColorIdx = 0; konstColorIdx < static_cast< u32 >(gx::TevKColorId::Max);
         konstColorIdx++) {
      *colorComp.GetTevKonstColor(i, static_cast< gx::TevKColorId >(konstColorIdx)) =
          math::ChirpColor{inStream.readUByte(), inStream.readUByte(), inStream.readUByte(),
                           inStream.readUByte()};
    }

    // Read swap tables.
    for (u32 swapTableIdx = 0; swapTableIdx < static_cast< u32 >(gx::TevSwapSel::SwapSel_Max);
         swapTableIdx++) {
      *colorComp.GetTevSwapTable(i, static_cast< gx::TevSwapSel >(swapTableIdx)) = {
          static_cast< gx::TevColorChan >(inStream.readUByte()),
          static_cast< gx::TevColorChan >(inStream.readUByte()),
          static_cast< gx::TevColorChan >(inStream.readUByte()),
          static_cast< gx::TevColorChan >(inStream.readUByte())};
    }

    // No padding to skip.
  }

  return true;
}

bool ChirpBinaryMaterials::ReadTextureComponent(SectionReader& inStream,
                                                gx::material::TextureComponent& texComp) {
  u16 texNameCount = inStream.readUint16();
  u16 texGenCount = inStream.readUint16();
  u16 texMtxCount = inStream.readUint16();
  inStream.seekAlign4();

  u32 firstTexNameOffset = inStream.readUint32();
  u32 firstTexGenOffset = inStream.readUint32();
  u32 firstTexMtxOffset = inStream.readUint32();

  // Read texture names.
  SeekToChunk(inStream, static_cast< u8 >(ChunkId::TextureNames), firstTexNameOffset);
  for (u32 texIdx = 0; texIdx < texNameCount; texIdx++) {
    u16 texNameSize = inStream.readUint16();
    inStream.seekAlign4();
    u32 texNameOffset = inStream.readUint32();

    auto nextNamePos =
        SeekToChunk(inStream, static_cast< u8 >(ChunkId::StringTable), texNameOffset);
    texComp.AddTexture(inStream.readString(texNameSize));

    inStream.seek(nextNamePos, athena::SeekOrigin::Begin);
  }

  // Read tex coord gens.
  SeekToChunk(inStream, static_cast< u8 >(ChunkId::TexCoordGens), firstTexGenOffset);
  for (u16 texGenIdx = 0; texGenIdx < texGenCount; texGenIdx++) {
    *texComp.AddTexCoordGen() = {static_cast< gx::TexGenType >(inStream.readUByte()),
                                 static_cast< gx::TexGenSrc >(inStream.readUByte()),
                                 static_cast< gx::TexMtx >(inStream.readUByte())};

    // Skip 1 padding byte.
    inStream.seekAlign4();
  }

  // Read texture matrices.
  SeekToChunk(inStream, static_cast< u8 >(ChunkId::TexCoordMatrices), firstTexMtxOffset);
  for (u16 texMtxIdx = 0; texMtxIdx < texMtxCount; texMtxIdx++) {
    gx::material::TexCoordMatrix& newTexMtx = *texComp.AddTexCoordMatrix();
    newTexMtx.m_type = static_cast< gx::TexMtxType >(inStream.readUByte());

    // Skip 3 padding bytes.
    inStream.seekAlign4();

    newTexMtx.m_mtx = inStream.readMatrix3x4();

    // No padding to skip.
  }

  return true;
}

bool ChirpBinaryMaterials::ReadLightingCompEntries(SectionReader& inStream,
                                                   gx::material::LightingComponent& lightingComp,
                                                   u32 lightChannelCount) {
  if (lightChannelCount == 0 || lightChannelCount >= gx::material::kMaxLightChannelCount) {
    return false;
  }

  for (u32 i = 0; i < lightChannelCount; i++) {
    gx::material::LightChannel& lightChannel =
        *lightingComp.AddChannel(static_cast< gx::ChannelId >(inStream.readUByte()));

    lightChannel.m_ambientColorSrc = static_cast< gx::ColorSrc >(inStream.readUByte());
    lightChannel.m_materialColorSrc = static_cast< gx::ColorSrc >(inStream.readUByte());
    lightChannel.m_lightMask = static_cast< u32 >(inStream.readUByte());
    lightChannel.m_diffuseFunc = static_cast< gx::DiffuseFn >(inStream.readUByte());
    lightChannel.m_attenuationFunc = static_cast< gx::AttenuationFn >(inStream.readUByte());
    inStream.seekAlign4();

    // Read ambient colors.
    for (u32 j = 0; j < gx::material::kMaxLightChannelCount; j++) {
      lightChannel.m_ambientColors[j] = {inStream.readUByte(), inStream.readUByte(),
                                         inStream.readUByte(), inStream.readUByte()};
    }

    // Read material colors.
    for (u32 j = 0; j < gx::material::kMaxLightChannelCount; j++) {
      lightChannel.m_materialColors[j] = {inStream.readUByte(), inStream.readUByte(),
                                          inStream.readUByte(), inStream.readUByte()};
    }
  }

  return true;
}

bool ChirpBinaryMaterials::ReadIndirectTexComponent(
    SectionReader& inStream, gx::material::IndirectTexComponent& indirectTexComp) {
  return true;
}

bool ChirpBinaryMaterials::ReadPixelEngineComponent(
    SectionReader& inStream, gx::material::PixelEngineComponent& pixelEngineComp) {
  pixelEngineComp.GetColorWritesEnabled() = inStream.readBool();
  pixelEngineComp.GetAlphaWritesEnabled() = inStream.readBool();
  pixelEngineComp.GetDitheringEnabled() = inStream.readBool();
  pixelEngineComp.GetCullMode() = static_cast< gx::CullMode >(inStream.readUByte());

  u32 alphaCompareOffset = inStream.readUint32();
  u32 blendModeOffset = inStream.readUint32();
  u32 fogOffset = inStream.readUint32();
  u32 zModeOffset = inStream.readUint32();

  // Read alpha compare.
  SeekToChunk(inStream, static_cast< u8 >(ChunkId::AlphaCompares), alphaCompareOffset);
  pixelEngineComp.GetAlphaCompare() = {
      static_cast< gx::Compare >(inStream.readUByte()), inStream.readUByte(),
      static_cast< gx::LogicOp >(inStream.readUByte()),
      static_cast< gx::Compare >(inStream.readUByte()), inStream.readUByte()};

  // Read blend mode.
  SeekToChunk(inStream, static_cast< u8 >(ChunkId::BlendModes), blendModeOffset);
  pixelEngineComp.GetBlendMode() = {static_cast< gx::BlendMode >(inStream.readUByte()),
                                    static_cast< gx::BlendFactor >(inStream.readUByte()),
                                    static_cast< gx::BlendFactor >(inStream.readUByte()),
                                    static_cast< gx::LogicOp >(inStream.readUByte())};

  // Read fog.
  SeekToChunk(inStream, static_cast< u8 >(ChunkId::Fog), fogOffset);
  {
    gx::material::Fog& fog = pixelEngineComp.GetFog();
    fog.m_type = static_cast< gx::FogType >(inStream.readUByte());

    fog.m_rangeAdjustEnabled = inStream.readBool();
    fog.m_rangeAdjustCenter = inStream.readUint16();

    fog.m_startZ = inStream.readFloat();
    fog.m_endZ = inStream.readFloat();
    fog.m_nearZ = inStream.readFloat();
    fog.m_farZ = inStream.readFloat();
    fog.m_color = {inStream.readUByte(), inStream.readUByte(), inStream.readUByte(),
                   inStream.readUByte()};

    for (u32 i = 0; i < 10; i++) {
      fog.m_rangeAdjustValues[i] = inStream.readUint16();
    }
  }

  // Read ZMode.
  SeekToChunk(inStream, static_cast< u8 >(ChunkId::ZModes), zModeOffset);
  pixelEngineComp.GetZMode() = {inStream.readBool(),
                                static_cast< gx::Compare >(inStream.readUByte()),
                                inStream.readBool(), inStream.readBool()};

  return true;
}

s64 ChirpBinaryMaterials::SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset) {
  ChunkId chunk = static_cast< ChunkId >(chunkId);

  u32 chunkOffset{0};
  switch (chunk) {
  case ChunkId::CombinerComponents:
    chunkOffset = m_combinerCompEntriesOffset;
    break;
  case ChunkId::ColorComponents:
    chunkOffset = m_colorCompEntriesOffset;
    break;
  case ChunkId::TextureComponents:
    chunkOffset = m_textureComponentsOffset;
    break;
  case ChunkId::TextureNames:
    chunkOffset = m_texNamesOffset;
    break;
  case ChunkId::TexCoordGens:
    chunkOffset = m_texCoordGensOffset;
    break;
  case ChunkId::TexCoordMatrices:
    chunkOffset = m_texCoordMatricesOffset;
    break;
  case ChunkId::LightingComponents:
    chunkOffset = m_lightingCompEntriesOffset;
    break;
  case ChunkId::PixelEngineComponents:
    chunkOffset = m_pixelEngineComponentsOffset;
    break;
  case ChunkId::AlphaCompares:
    chunkOffset = m_alphaComparesOffset;
    break;
  case ChunkId::BlendModes:
    chunkOffset = m_blendModesOffset;
    break;
  case ChunkId::Fog:
    chunkOffset = m_fogOffset;
    break;
  case ChunkId::ZModes:
    chunkOffset = m_zModesOffset;
    break;
  case ChunkId::IndirectTexComponents:
    chunkOffset = m_indirectTexComponentsOffset;
    break;
  case ChunkId::StringTable:
    chunkOffset = m_stringTableOffset;
    break;
  default:
    break;
  }

  s64 curPos = sectionReader.position();
  sectionReader.seek(m_offset + chunkOffset + dataOffset, athena::SeekOrigin::Begin);

  return curPos;
}

std::unique_ptr< ChirpBinarySection > ChirpBinaryMaterials::CreateMATSection(u32 size,
                                                                             u32 elementCount,
                                                                             u16 versionMajor,
                                                                             u16 versionMinor) {
  return std::make_unique< ChirpBinaryMaterials >(size, elementCount, versionMajor, versionMinor);
}

std::unique_ptr< ChirpBinarySection >
ChirpBinaryMaterials::CreateMATSectionWithOffset(u64 offset, u32 size, u32 elementCount,
                                                 u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinaryMaterials >(offset, size, elementCount, versionMajor,
                                                  versionMinor);
}
} // namespace chirp::binary
