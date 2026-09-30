#include "libchirp-binary/section/ChirpBinaryVat.hpp"

#include "libchirp-binary/util/VertexUtils.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>
#include <cmath>

namespace chirp::binary {
ChirpBinaryVat::ChirpBinaryVat() : ChirpBinarySection() {}

ChirpBinaryVat::ChirpBinaryVat(u32 size, u32 elementCount)
: ChirpBinarySection(ChirpSectionType::Vertex, size, elementCount) {}

ChirpBinaryVat::ChirpBinaryVat(u64 offset, u32 size, u32 elementCount)
: ChirpBinarySection(ChirpSectionType::Vertex, offset, size, elementCount) {}

bool ChirpBinaryVat::Import(athena::io::IStreamReader& inStream) {
  u32 dataOffset = inStream.readUint32();

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

    u64 curPos = inStream.position();
    inStream.seek(m_offset + dataOffset + attrOffset, athena::SeekOrigin::Begin);

    // TODO: Make endianness toggleable via ChirpBinaryImportOptions?
    inStream.setEndian(athena::Endian::Big);
    ReadVertexAttributeData(inStream, attrCount, newAttribute);
    inStream.setEndian(athena::Endian::Little);

    inStream.seek(curPos, athena::SeekOrigin::Begin);
  }

  return true;
}

bool ChirpBinaryVat::Export(athena::io::IStreamWriter& outStream) { return true; }

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

void ChirpBinaryVat::WriteVertexAttributeData(athena::io::IStreamWriter& outStream, u32 count,
                                              ChirpVertexAttribute* attribute) {}

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
                                                                       const u32 elementCount) {
  return std::make_unique< ChirpBinaryVat >(ChirpBinaryVat(size, elementCount));
}
std::unique_ptr< ChirpBinarySection >
ChirpBinaryVat::CreateVATSectionWithOffset(const u64 offset, const u32 size,
                                           const u32 elementCount) {
  return std::make_unique< ChirpBinaryVat >(ChirpBinaryVat(offset, size, elementCount));
}
} // namespace chirp::binary