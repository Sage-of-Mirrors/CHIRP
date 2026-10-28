#include "libchirp/section/ChirpBinarySkin.hpp"
#include "libchirp/math/ChirpVector.hpp"

#include <athena/VectorWriter.hpp>

namespace chirp::binary {
ChirpBinarySkin::ChirpBinarySkin() : ChirpBinarySection() {}
ChirpBinarySkin::ChirpBinarySkin(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor)
: ChirpBinarySection(size, elementCount, versionMajor, versionMinor) {}
ChirpBinarySkin::ChirpBinarySkin(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                 u16 versionMinor)
: ChirpBinarySection(offset, size, elementCount, versionMajor, versionMinor) {}

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

bool ChirpBinarySkin::Export(SectionWriter& outStream) {
  // Write section header.
  outStream.writeUint32(FourCC("CSKN").toUint32());
  outStream.writeUint16(m_versionMajor);
  outStream.writeUint16(m_versionMinor);
  outStream.writeUint32(0); // Placeholder for section size.
  outStream.writeUint32(static_cast< u32 >(m_bindings.size()));
  outStream.writeUint32(static_cast< u32 >(m_inverseBindMatrices.size()));
  outStream.writeUint32(0); // Placeholder for indices offset.
  outStream.writeUint32(0); // Placeholder for weights offset.
  outStream.writeUint32(0); // Placeholder for inverse bind matrices offset.

  athena::io::VectorWriter indicesWriter;
  indicesWriter.setEndian(athena::Endian::Little);
  athena::io::VectorWriter weightsWriter;
  weightsWriter.setEndian(athena::Endian::Little);

  // Write binding entries.
  for (u32 i = 0; i < m_bindings.size(); i++) {
    ChirpSkinBinding* curBinding = m_bindings[i].get();
    u16 curBindingCount = curBinding->GetBindingCount();

    outStream.writeUint16(curBindingCount);
    outStream.writeAlign4();
    outStream.writeUint32(static_cast< u32 >(indicesWriter.length()));
    outStream.writeUint32(static_cast< u32 >(weightsWriter.length()));

    ChirpSkinIndices& bindingIndices = curBinding->GetIndices();
    ChirpSkinWeights& bindingWeights = curBinding->GetWeights();
    for (u32 b = 0; b < curBindingCount; b++) {
      indicesWriter.writeUint16(bindingIndices[b]);
      weightsWriter.writeFloat(bindingWeights[b]);
    }
  }

  // Write accumulated indices.
  outStream.writeAlign16();
  outStream.writeOffset(kIndicesOffsetPos);
  outStream.writeUBytes(indicesWriter.data().data(), indicesWriter.length());

  // Write accumulated weights.
  outStream.writeAlign16();
  outStream.writeOffset(kWeightsOffsetPos);
  outStream.writeUBytes(weightsWriter.data().data(), weightsWriter.length());

  // Write inverse bind matrices.
  outStream.writeAlign16();
  outStream.writeOffset(kMatricesOffsetPos);
  for (const math::ChirpMatrix3x4& mtx : m_inverseBindMatrices) {
    outStream.writeVector4(mtx.GetRow(0));
    outStream.writeVector4(mtx.GetRow(1));
    outStream.writeVector4(mtx.GetRow(2));
  }

  outStream.writeSectionSize();
  return true;
}

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
  return std::make_unique< ChirpBinarySkin >(size, elementCount, versionMajor, versionMinor);
}
std::unique_ptr< ChirpBinarySection >
ChirpBinarySkin::CreateSKNSectionWithOffset(u64 offset, u32 size, u32 elementCount,
                                            u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinarySkin >(offset, size, elementCount, versionMajor,
                                             versionMinor);
}
} // namespace chirp::binary