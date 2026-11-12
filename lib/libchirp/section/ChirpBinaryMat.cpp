#include "libchirp/section/ChirpBinaryMat.hpp"
#include "libchirp/math/ChirpColor.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>

namespace {
using namespace chirp::binary;
using namespace chirp::gx;

struct MaterialBinaryEntry {
  /*0x00*/ u16 m_nameSize;
  /*0x02*/ u16 m_tevStageCount;
  /*0x04*/ u16 m_lightChannelCount;
  /*0x06*/ u16 m_pad0;

  /*0x08*/ u32 m_nameOffset;
  /*0x0C*/ u32 m_firstCombinerCompEntryOffset;
  /*0x10*/ u32 m_firstColorCompEntryOffset;
  /*0x14*/ u32 m_textureComponentOffset;
  /*0x18*/ u32 m_firstLightingCompEntryOffset;
  /*0x1C*/ u32 m_indirectTexComponentOffset;
  /*0x20*/ u32 m_pixelEngineComponentOffset;
};

MaterialBinaryEntry ReadMaterialData(SectionReader& sectionReader) {
  MaterialBinaryEntry matData;
  outData.m_nameSize = sectionReader.readUint16();
  outData.m_stageCount = sectionReader.readUint16();
  outData.m_colorUpdate = sectionReader.readBool();
  outData.m_alphaUpdate = sectionReader.readBool();
  outData.m_zCompLoc = sectionReader.readBool();
  outData.m_dither = sectionReader.readBool();

  outData.m_nameOffset = sectionReader.readUint32();
  outData.m_firstStageOffset = sectionReader.readUint32();
  outData.m_alphaCompareOffset = sectionReader.readUint32();
  outData.m_blendModeOffset = sectionReader.readUint32();
  outData.m_fogOffset = sectionReader.readUint32();
  outData.m_indirectStagesOffset = sectionReader.readUint32();
  outData.m_zModeOffset = sectionReader.readUint32();
  outData.m_konstSelsOffset = sectionReader.readUint32();
  outData.m_tevColorsOffset = sectionReader.readUint32();
  outData.m_konstColorsOffset = sectionReader.readUint32();

  return matData;
}

void ReadAlphaCompare(SectionReader& sectionReader, AlphaCompare& outAlphaComp) {
  outAlphaComp.m_compA = static_cast< Compare >(sectionReader.readUByte());
  outAlphaComp.m_refA = sectionReader.readUByte();
  outAlphaComp.m_op = static_cast< AlphaOp >(sectionReader.readUByte());
  outAlphaComp.m_compB = static_cast< Compare >(sectionReader.readUByte());
  outAlphaComp.m_refB = sectionReader.readUByte();
}
void ReadBlendMode(SectionReader& sectionReader, BlendMode& outBlendMode) {
  outBlendMode.m_type = static_cast< BlendType >(sectionReader.readUByte());
  outBlendMode.m_srcFactor = static_cast< BlendFactor >(sectionReader.readUByte());
  outBlendMode.m_dstFactor = static_cast< BlendFactor >(sectionReader.readUByte());
  outBlendMode.m_op = static_cast< LogicOp >(sectionReader.readUByte());
}

void ReadFog(SectionReader& sectionReader, Fog& outFog) {
  outFog.m_type = static_cast< FogType >(sectionReader.readUByte());
  sectionReader.seekAlign4();

  outFog.m_startZ = sectionReader.readFloat();
  outFog.m_endZ = sectionReader.readFloat();
  outFog.m_nearZ = sectionReader.readFloat();
  outFog.m_farZ = sectionReader.readFloat();

  outFog.m_color = chirp::math::ChirpColor(sectionReader.readUByte(), sectionReader.readUByte(),
                                           sectionReader.readUByte(), sectionReader.readUByte());

  outFog.m_rangeAdjust.m_enabled = sectionReader.readBool();
  sectionReader.seek(1);

  outFog.m_rangeAdjust.m_center = sectionReader.readUint16();

  for (u32 i = 0; i < 10; i++) {
    outFog.m_rangeAdjust.m_adjTable[i] = sectionReader.readUint16();
  }
}

void ReadZMode(SectionReader& sectionReader, ZMode& outZMode) {
  outZMode.m_compareEnable = sectionReader.readBool();
  outZMode.m_func = static_cast< Compare >(sectionReader.readUByte());
  outZMode.m_updateEnable = sectionReader.readBool();
}

void ReadTevStage(SectionReader& sectionReader, TevStage& outStage) {
  // Color combiner info.
  outStage.m_colorCombiner.m_argA = static_cast< TevColorArg >(sectionReader.readUByte()); // 0x00
  outStage.m_colorCombiner.m_argB = static_cast< TevColorArg >(sectionReader.readUByte()); // 0x01
  outStage.m_colorCombiner.m_argC = static_cast< TevColorArg >(sectionReader.readUByte()); // 0x02
  outStage.m_colorCombiner.m_argD = static_cast< TevColorArg >(sectionReader.readUByte()); // 0x03
  outStage.m_colorCombiner.m_op = static_cast< TevOp >(sectionReader.readUByte());         // 0x04
  outStage.m_colorCombiner.m_bias = static_cast< TevBias >(sectionReader.readUByte());     // 0x05
  outStage.m_colorCombiner.m_scale = static_cast< TevScale >(sectionReader.readUByte());   // 0x06
  outStage.m_colorCombiner.m_outReg = static_cast< TevRegId >(sectionReader.readUByte());  // 0x07
  outStage.m_colorCombiner.m_clamp = sectionReader.readBool();                             // 0x08

  // Alpha combiner info.
  outStage.m_alphaCombiner.m_argA = static_cast< TevAlphaArg >(sectionReader.readUByte()); // 0x09
  outStage.m_alphaCombiner.m_argB = static_cast< TevAlphaArg >(sectionReader.readUByte()); // 0x0A
  outStage.m_alphaCombiner.m_argC = static_cast< TevAlphaArg >(sectionReader.readUByte()); // 0x0B
  outStage.m_alphaCombiner.m_argD = static_cast< TevAlphaArg >(sectionReader.readUByte()); // 0x0C
  outStage.m_alphaCombiner.m_op = static_cast< TevOp >(sectionReader.readUByte());         // 0x0D
  outStage.m_alphaCombiner.m_bias = static_cast< TevBias >(sectionReader.readUByte());     // 0x0E
  outStage.m_alphaCombiner.m_scale = static_cast< TevScale >(sectionReader.readUByte());   // 0x0F
  outStage.m_alphaCombiner.m_outReg = static_cast< TevRegId >(sectionReader.readUByte());  // 0x10
  outStage.m_alphaCombiner.m_clamp = sectionReader.readBool();                             // 0x11

  // Tev order info (config for texture sampling and/or raster color sampling).
  outStage.m_tevOrder.m_texMapId = static_cast< TexMapId >(sectionReader.readUByte());     // 0x12
  outStage.m_tevOrder.m_texCoordId = static_cast< TexCoordId >(sectionReader.readUByte()); // 0x13
  outStage.m_tevOrder.m_channelId = static_cast< ChannelId >(sectionReader.readUByte());   // 0x14

  sectionReader.seekAlign4(); // Skip padding bytes at 0x15, 0x16, and 0x17.
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
  // ReadOffsets(inStream);

  // for (u32 i = 0; i < m_elementCount; i++) {
  //   ReadMaterial(inStream);
  // }

  return true;
}

bool ChirpBinaryMaterials::Export(SectionWriter& outStream) { return true; }

gx::material::Material* ChirpBinaryMaterials::GetMaterial(const std::string& name) const {
  // if (auto itr = m_materials.find(name); itr != m_materials.end()) {
  //   return itr->second.get();
  // }

  // std::cerr << "Attempted to get a material named \"" << name
  //           << "\" from a ChirpBinaryMaterials that did not have a material with that name."
  //           << std::endl;
  return nullptr;
}

gx::material::Material* ChirpBinaryMaterials::CreateMaterial(const std::string& name,
                                                             bool colorUpdate, bool alphaUpdate,
                                                             bool zCompLoc, bool dither) {
  return nullptr;
  // if (m_materials.contains(name)) {
  //   std::cerr << "Attempted to add material named \"" << name
  //             << "\" to ChirpBinaryMaterials that already had a material with that name."
  //             << std::endl;
  //   return nullptr;
  // }

  // m_materials[name] =
  //     std::make_unique< gx::material::Material >(name, colorUpdate, alphaUpdate, zCompLoc,
  //     dither);
  // return m_materials[name].get();
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
                                                   material::CombinerComponent& combinerComp,
                                                   u32 tevStageCount) {
  if (tevStageCount == 0 || tevStageCount > kMaxTevStageCount) {
    return false;
  }

  for (u32 i = 0; i < tevStageCount; i++) {
    material::TevStage* newStage = combinerComp.AddTevStage();
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
  if (tevStageCount == 0 || tevStageCount > kMaxTevStageCount) {
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
      math::ChirpColorS10* regColor =
          colorComp.GetTevRegColor(i, static_cast< gx::TevRegId >(regColorIdx));
      *regColor = math::ChirpColorS10{inStream.readInt16(), inStream.readInt16(),
                                      inStream.readInt16(), inStream.readInt16()};
    }

    // Read konst colors.
    for (u32 konstColorIdx = 0; konstColorIdx < static_cast< u32 >(gx::TevKColorId::Max);
         konstColorIdx++) {
      math::ChirpColor* konstColor =
          colorComp.GetTevKonstColor(i, static_cast< gx::TevKColorId >(konstColorIdx));
      *konstColor = math::ChirpColor{inStream.readUByte(), inStream.readUByte(),
                                     inStream.readUByte(), inStream.readUByte()};
    }

    // Read swap tables.
    for (u32 swapTableIdx = 0; swapTableIdx < static_cast< u32 >(gx::TevSwapSel::SwapSel_Max);
         swapTableIdx++) {
      gx::material::TevSwapTable& swapTable =
          *colorComp.GetTevSwapTable(i, static_cast< gx::TevSwapSel >(swapTableIdx));

      swapTable[0] = static_cast< gx::TevColorChan >(inStream.readUByte());
      swapTable[1] = static_cast< gx::TevColorChan >(inStream.readUByte());
      swapTable[2] = static_cast< gx::TevColorChan >(inStream.readUByte());
      swapTable[3] = static_cast< gx::TevColorChan >(inStream.readUByte());
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
  SeekToChunk(inStream, static_cast< u8 >(ChunkId::TexNames), firstTexNameOffset);
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

    nwTexMtx.m_mtx = inStream.readMatrix3x4();

    // No padding to skip.
  }

  return true;
}

bool ChirpBinaryMaterials::Import(SectionReader& inStream) {
  ReadOffsets(inStream);
  inStream.seekAlign16();

  for (u32 i = 0; i < m_elementCount; i++) {
    MaterialBinaryEntry matData = ReadMaterialData(inStream);

    auto nextMatPos =
        SeekToChunk(inStream, static_cast< u8 >(ChunkId::StringTable), matData.m_nameOffset);
    std::string newMatName = inStream.readString(matData.m_nameSize);

    gx::Material* newMaterial = CreateMaterial(newMatName);
    if (!newMaterial) {
      std::cout << "Failed to create material named \"" << newMatName << "\"." << std::endl;
      return false;
    }

    // Read in the combiner component.
    SeekToChunk(inStream, static_cast< u8 >(ChunkId::CombinerComponents),
                matData.m_firstCombinerCompEntry);
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
}

s64 ChirpBinaryMaterials::SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset) {
  return 0;
  // ChunkId chunk = static_cast< ChunkId >(chunkId);

  // u32 chunkOffset{0};
  // switch (chunk) {
  // case ChunkId::AlphaCompares:
  //   chunkOffset = m_alphaComparesOffset;
  //   break;
  // case ChunkId::BlendModes:
  //   chunkOffset = m_blendModesOffset;
  //   break;
  // case ChunkId::Fog:
  //   chunkOffset = m_fogOffset;
  //   break;
  // case ChunkId::IndirectStages:
  //   chunkOffset = m_indirectStagesOffset;
  //   break;
  // case ChunkId::MaterialNames:
  //   chunkOffset = m_materialNamesOffset;
  //   break;
  // case ChunkId::Stages:
  //   chunkOffset = m_stagesOffset;
  //   break;
  // case ChunkId::ZModes:
  //   chunkOffset = m_zmodesOffset;
  //   break;
  // case ChunkId::KonstSels:
  //   chunkOffset = m_konstSelsOffset;
  //   break;
  // case ChunkId::TevColors:
  //   chunkOffset = m_tevColorsOffset;
  //   break;
  // case ChunkId::KonstColors:
  //   chunkOffset = m_konstColorsOffset;
  //   break;
  // default:
  //   break;
  // }

  // s64 curPos = sectionReader.position();
  // sectionReader.seek(m_offset + chunkOffset + dataOffset, athena::SeekOrigin::Begin);

  // return curPos;
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
