#include "libchirp/section/ChirpBinaryMat.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>

namespace {
using namespace chirp::binary;
using namespace chirp::gx;

struct MaterialData {
  /*0x00*/ u16 m_nameSize;
  /*0x02*/ u16 m_stageCount;
  /*0x04*/ bool m_colorUpdate;
  /*0x05*/ bool m_alphaUpdate;
  /*0x06*/ bool m_zCompLoc;
  /*0x07*/ bool m_dither;

  /*0x08*/ u32 m_nameOffset;
  /*0x0C*/ u32 m_firstStageOffset;
  /*0x10*/ u32 m_alphaCompareOffset;
  /*0x14*/ u32 m_blendModeOffset;
  /*0x18*/ u32 m_fogOffset;
  /*0x1C*/ u32 m_indirectStagesOffset;
  /*0x20*/ u32 m_zModeOffset;
};

void ReadMaterialData(SectionReader& sectionReader, MaterialData& outData) {
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
: ChirpBinarySection(ChirpSectionType::Materials, size, elementCount, versionMajor, versionMinor) {}
ChirpBinaryMaterials::ChirpBinaryMaterials(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                           u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Materials, offset, size, elementCount, versionMajor,
                     versionMinor) {}

bool ChirpBinaryMaterials::Import(SectionReader& inStream) {
  ReadOffsets(inStream);

  for (u32 i = 0; i < m_elementCount; i++) {
    ReadMaterial(inStream);
  }

  return true;
}

bool ChirpBinaryMaterials::Export(athena::io::IStreamWriter& outStream) { return true; }

gx::Material* ChirpBinaryMaterials::GetMaterial(std::string_view name) const {
  if (auto itr = m_materials.find(std::string(name)); itr != m_materials.end()) {
    return itr->second.get();
  }

  std::cerr << "Attempted to get a material named \"" << name
            << "\" from a ChirpBinaryMaterials that did not have a material with that name."
            << std::endl;
  return nullptr;
}

gx::Material* ChirpBinaryMaterials::CreateMaterial(std::string_view name, bool colorUpdate,
                                                   bool alphaUpdate, bool zCompLoc, bool dither) {
  std::string nameAsStr(name);

  if (m_materials.contains(nameAsStr)) {
    std::cerr << "Attempted to add material named \"" << name
              << "\" to ChirpBinaryMaterials that already had a material with that name."
              << std::endl;
    return nullptr;
  }

  m_materials[nameAsStr] =
      std::make_unique< gx::Material >(name, colorUpdate, alphaUpdate, zCompLoc, dither);
  return m_materials[nameAsStr].get();
}

void ChirpBinaryMaterials::ReadOffsets(SectionReader& sectionReader) {
  m_materialNamesOffset = sectionReader.readUint32();  // 0x10
  m_alphaComparesOffset = sectionReader.readUint32();  // 0x14
  m_blendModesOffset = sectionReader.readUint32();     // 0x18
  m_fogOffset = sectionReader.readUint32();            // 0x1C
  m_stagesOffset = sectionReader.readUint32();         // 0x20
  m_indirectStagesOffset = sectionReader.readUint32(); // 0x24
  m_zmodesOffset = sectionReader.readUint32();         // 0x28

  sectionReader.seekAlign16();
}

void ChirpBinaryMaterials::ReadMaterial(SectionReader& sectionReader) {
  MaterialData rawData;
  ReadMaterialData(sectionReader, rawData);

  s64 nextMatPos =
      SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::MaterialNames), rawData.m_nameOffset);
  Material* newMaterial =
      CreateMaterial(sectionReader.readString(rawData.m_nameSize), rawData.m_colorUpdate,
                     rawData.m_alphaUpdate, rawData.m_zCompLoc, rawData.m_dither);

  // Alpha compare.
  SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::AlphaCompares),
              rawData.m_alphaCompareOffset);
  ReadAlphaCompare(sectionReader, newMaterial->GetAlphaCompare());

  // Blend mode.
  SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::BlendModes), rawData.m_blendModeOffset);
  ReadBlendMode(sectionReader, newMaterial->GetBlendMode());

  // Z-mode.
  SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::ZModes), rawData.m_zModeOffset);
  ReadZMode(sectionReader, newMaterial->GetZMode());

  // TEV stages.
  SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Stages), rawData.m_firstStageOffset);
  for (u32 s = 0; s < rawData.m_stageCount; s++) {
    TevStage* newStage = newMaterial->CreateStage();
    ReadTevStage(sectionReader, *newStage);
  }

  // Fog, if present.
  if (rawData.m_fogOffset != std::numeric_limits< u32 >::max()) {
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Fog), rawData.m_fogOffset);
    ReadFog(sectionReader, newMaterial->GetFog());
  }

  // Indirect texture stages, if present.
  if (rawData.m_indirectStagesOffset != std::numeric_limits< u32 >::max()) {
    SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::IndirectStages),
                rawData.m_indirectStagesOffset);
    // TODO: indirect stage reading.
  }

  sectionReader.seek(nextMatPos, athena::SeekOrigin::Begin);
}

s64 ChirpBinaryMaterials::SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset) {
  ChunkId chunk = static_cast< ChunkId >(chunkId);

  u32 chunkOffset{0};
  switch (chunk) {
  case ChunkId::AlphaCompares:
    chunkOffset = m_alphaComparesOffset;
    break;
  case ChunkId::BlendModes:
    chunkOffset = m_blendModesOffset;
    break;
  case ChunkId::Fog:
    chunkOffset = m_fogOffset;
    break;
  case ChunkId::IndirectStages:
    chunkOffset = m_indirectStagesOffset;
    break;
  case ChunkId::MaterialNames:
    chunkOffset = m_materialNamesOffset;
    break;
  case ChunkId::Stages:
    chunkOffset = m_stagesOffset;
    break;
  case ChunkId::ZModes:
    chunkOffset = m_zmodesOffset;
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
