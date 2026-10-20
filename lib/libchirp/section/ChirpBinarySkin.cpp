#include "libchirp/section/ChirpBinarySkin.hpp"
#include "libchirp/math/ChirpVector.hpp"

#include <athena/IStreamWriter.hpp>

namespace chirp::binary {
ChirpBinarySkin::ChirpBinarySkin() : ChirpBinarySection() {}
ChirpBinarySkin::ChirpBinarySkin(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Skinning, size, elementCount, versionMajor, versionMinor) {}
ChirpBinarySkin::ChirpBinarySkin(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                 u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Skinning, offset, size, elementCount, versionMajor,
                     versionMinor) {}

math::ChirpMatrix3x4& ChirpBinarySkin::AddInverseBindMatrix() {
  m_inverseBindMatrices.emplace_back();
  return m_inverseBindMatrices.back();
}

math::ChirpMatrix3x4& ChirpBinarySkin::AddInverseBindMatrix(const math::ChirpVector4& r0,
                                                            const math::ChirpVector4& r1,
                                                            const math::ChirpVector4& r2) {
  m_inverseBindMatrices.emplace_back(r0, r1, r2);
  return m_inverseBindMatrices.back();
}

bool ChirpBinarySkin::Import(SectionReader& sectionReader) {
  m_inverseBindMatrixCount = sectionReader.readUint32();
  m_indicesOffset = sectionReader.readUint32();
  m_weightsOffset = sectionReader.readUint32();
  m_matricesOffset = sectionReader.readUint32();

  for (u32 i = 0; i < m_elementCount; i++) {
    u16 bindCount = sectionReader.readUint16();
    sectionReader.seekAlign4();

    u32 firstIndexOffset = sectionReader.readUint32();
    u32 firstWeightOffset = sectionReader.readUint32();

    s64 nextBindingPos = sectionReader.position();

    ChirpSkinIndices indices;
    ChirpSkinWeights weights;
    for (u16 b = 0; b < bindCount; b++) {
      SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Indices),
                  firstIndexOffset + (b * sizeof(u16)));
      indices[b] = sectionReader.readUint16();

      SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Weights),
                  firstWeightOffset + (b * sizeof(f32)));
      weights[b] = sectionReader.readFloat();
    }

    CreateSkinBinding(bindCount, indices, weights);
    sectionReader.seek(nextBindingPos, athena::SeekOrigin::Begin);
  }

  SeekToChunk(sectionReader, static_cast< u8 >(ChunkId::Matrices), 0);

  for (u32 i = 0; i < m_inverseBindMatrixCount; i++) {
    math::ChirpVector4 r0{sectionReader.readFloat(), sectionReader.readFloat(),
                          sectionReader.readFloat(), sectionReader.readFloat()};
    math::ChirpVector4 r1{sectionReader.readFloat(), sectionReader.readFloat(),
                          sectionReader.readFloat(), sectionReader.readFloat()};
    math::ChirpVector4 r2{sectionReader.readFloat(), sectionReader.readFloat(),
                          sectionReader.readFloat(), sectionReader.readFloat()};

    AddInverseBindMatrix(r0, r1, r2);
  }

  return true;
}

bool ChirpBinarySkin::Export(SectionWriter& outStream) { return true; }

s64 ChirpBinarySkin::SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset) {
  ChunkId chunk = static_cast< ChunkId >(chunkId);

  u32 chunkOffset{0};
  switch (chunk) {
  case ChunkId::Indices:
    chunkOffset = m_indicesOffset;
    break;
  case ChunkId::Weights:
    chunkOffset = m_weightsOffset;
    break;
  case ChunkId::Matrices:
    chunkOffset = m_matricesOffset;
    break;
  default:
    break;
  }

  s64 curPos = sectionReader.position();
  sectionReader.seek(m_offset + chunkOffset + dataOffset, athena::SeekOrigin::Begin);

  return curPos;
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