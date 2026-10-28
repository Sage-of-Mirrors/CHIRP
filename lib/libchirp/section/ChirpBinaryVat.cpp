#include "libchirp/section/ChirpBinaryVat.hpp"
#include "libchirp/util/VertexUtils.hpp"

#include <athena/VectorWriter.hpp>

#include <cmath>

namespace chirp::binary {
ChirpBinaryVat::ChirpBinaryVat() : ChirpBinarySection() {}

ChirpBinaryVat::ChirpBinaryVat(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor)
: ChirpBinarySection(size, elementCount, versionMajor, versionMinor) {}

ChirpBinaryVat::ChirpBinaryVat(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                               u16 versionMinor)
: ChirpBinarySection(offset, size, elementCount, versionMajor, versionMinor) {}

bool ChirpBinaryVat::Import(SectionReader& inStream) {
  m_attributesOffset = inStream.readUint32();
  inStream.seekAlign16();

  for (u32 i = 0; i < m_elementCount; i++) {
    u32 attrCount = inStream.readUint32();
    u32 attrOffset = inStream.readUint32();

    gx::VtxAttribute attrType = static_cast< gx::VtxAttribute >(inStream.readUByte());
    gx::VtxComponentSize compSize = static_cast< gx::VtxComponentSize >(inStream.readUByte());
    gx::VtxComponentType compType = static_cast< gx::VtxComponentType >(inStream.readUByte());
    u8 exponent = inStream.readUByte();

    ChirpVertexAttribute* newAttribute =
        CreateVertexAttribute(attrType, compSize, compType, exponent);
    if (!newAttribute) {
      return false;
    }

    s64 returnPos = SeekToChunk(inStream, static_cast< u8 >(ChunkId::Attributes), attrOffset);

    // TODO: Make endianness toggleable via ChirpBinaryImportOptions?
    inStream.setEndian(athena::Endian::Big);
    ReadVertexAttributeData(inStream, attrCount, newAttribute);
    inStream.setEndian(athena::Endian::Little);

    inStream.seek(returnPos);
  }

  return true;
}

bool ChirpBinaryVat::Export(SectionWriter& outStream) {
  // Write section header.
  outStream.writeUint32(FourCC("CVAT").toUint32());
  outStream.writeUint16(m_versionMajor);
  outStream.writeUint16(m_versionMinor);
  outStream.writeUint32(0); // Placeholder for section size.
  outStream.writeUint32(static_cast< u32 >(m_attributes.size()));
  outStream.writeUint32(0); // Placeholder for attributes offset.
  outStream.writeAlign16();

  athena::io::VectorWriter attributeWriter;

  // Write vertex attributes.
  for (const auto& v : m_attributes) {
    outStream.writeUint32(static_cast< u32 >(v.second->GetData().size()));
    outStream.writeUint32(static_cast< u32 >(attributeWriter.length()));
    outStream.writeUByte(static_cast< u8 >(v.second->GetAttribute()));
    outStream.writeUByte(static_cast< u8 >(v.second->GetComponentSize()));
    outStream.writeUByte(static_cast< u8 >(v.second->GetComponentType()));
    outStream.writeUByte(static_cast< u8 >(v.second->GetExponent()));

    WriteVertexAttributeData(attributeWriter, v.second.get());
  }

  // Write attribute data.
  outStream.writeAlign16();
  outStream.writeOffset(kAttributesOffsetPos);
  outStream.writeUBytes(attributeWriter.data().data(), attributeWriter.length());

  outStream.writeAlign16();
  outStream.writeSectionSize();

  return true;
}

s64 ChirpBinaryVat::SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset) {
  ChunkId chunk = static_cast< ChunkId >(chunkId);

  u32 chunkOffset{0};
  switch (chunk) {
  case ChunkId::Attributes:
    chunkOffset = m_attributesOffset;
    break;
  default:
    break;
  }

  s64 curPos = sectionReader.position();
  sectionReader.seek(m_offset + chunkOffset + dataOffset, athena::SeekOrigin::Begin);

  return curPos;
}

void ChirpBinaryVat::ReadVertexAttributeData(athena::io::IStreamReader& inStream, u32 count,
                                             ChirpVertexAttribute* attribute) {
  if (!count || !attribute) {
    return;
  }

  u32 compSize = util::GetComponentCount(attribute->GetAttribute(), attribute->GetComponentSize());

  std::vector< math::ChirpVector4 >& attrData = attribute->GetData();
  attrData.resize(count);

  for (u32 i = 0; i < count; i++) {
    math::ChirpVector4 dataVec;

    for (u32 j = 0; j < compSize; j++) {
      attrData[i][j] = inStream.readFloat();
    }
  }
}

void ChirpBinaryVat::WriteVertexAttributeData(athena::io::IStreamWriter& outStream,
                                              ChirpVertexAttribute* attribute) {
  u32 compSize = util::GetComponentCount(attribute->GetAttribute(), attribute->GetComponentSize());

  std::vector< math::ChirpVector4 >& attrData = attribute->GetData();
  for (const auto& v : attrData) {
    for (u32 i = 0; i < compSize; i++) {
      outStream.writeFloat(v[i]);
    }
  }
}

ChirpVertexAttribute* ChirpBinaryVat::GetVertexAttribute(gx::VtxAttribute attr) {
  if (attr >= gx::VtxAttribute::VtxAttributeMax) {
    std::cerr << "Attempted to get an invalid attribute from ChirpBinaryVat." << std::endl;
    return nullptr;
  }

  if (auto itr = m_attributes.find(attr); itr != m_attributes.end()) {
    return itr->second.get();
  }

  std::cerr << "Attempted to get a non-existent vertex attribute from ChirpBinaryVat." << std::endl;
  return nullptr;
}

ChirpVertexAttribute* ChirpBinaryVat::CreateVertexAttribute(gx::VtxAttribute attr,
                                                            gx::VtxComponentSize size,
                                                            gx::VtxComponentType type,
                                                            u8 exponent) {
  if (attr >= gx::VtxAttribute::VtxAttributeMax) {
    std::cerr << "Attempted to create an invalid attribute in ChirpBinaryVat." << std::endl;
    return nullptr;
  }

  if (m_attributes.contains(attr)) {
    std::cerr << "Attempted to create an attribute in ChirpBinaryVat that it already contains."
              << std::endl;
    return nullptr;
  }

  m_attributes[attr] = std::make_unique< ChirpVertexAttribute >(attr, size, type, exponent);
  return m_attributes[attr].get();
}

ChirpVertexAttribute* ChirpBinaryVat::CreateVertexAttribute(
    gx::VtxAttribute attr, const std::vector< math::ChirpVector4 >& data, gx::VtxComponentSize size,
    gx::VtxComponentType type, u8 exponent) {
  if (attr >= gx::VtxAttribute::VtxAttributeMax) {
    std::cerr << "Attempted to create an invalid attribute in ChirpBinaryVat." << std::endl;
    return nullptr;
  }

  if (m_attributes.contains(attr)) {
    std::cerr << "Attempted to create an attribute in ChirpBinaryVat that it already contains."
              << std::endl;
    return nullptr;
  }

  m_attributes[attr] = std::make_unique< ChirpVertexAttribute >(attr, data, size, type, exponent);
  return m_attributes[attr].get();
}

std::unique_ptr< ChirpBinarySection > ChirpBinaryVat::CreateVATSection(const u32 size,
                                                                       const u32 elementCount,
                                                                       u16 versionMajor,
                                                                       u16 versionMinor) {
  return std::make_unique< ChirpBinaryVat >(size, elementCount, versionMajor, versionMinor);
}
std::unique_ptr< ChirpBinarySection >
ChirpBinaryVat::CreateVATSectionWithOffset(const u64 offset, const u32 size, const u32 elementCount,
                                           u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinaryVat >(offset, size, elementCount, versionMajor, versionMinor);
}
} // namespace chirp::binary