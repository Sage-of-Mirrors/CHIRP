#include "libchirp-binary/section/ChirpBinaryVat.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>

using namespace chirp;

namespace {
u32 GetComponentCount(gx::VtxAttribute attr, gx::VtxComponentSize compSize) {
  if (attr == gx::VtxAttribute::Position) {
    switch (compSize) {
    case gx::VtxComponentSize::PositionXy:
      return 2;
    case gx::VtxComponentSize::PositionXyz:
      return 3;
    default:
      return 0;
    }
  } else if (attr == gx::VtxAttribute::Normal) {
    switch (compSize) {
    case gx::VtxComponentSize::NormalXyz:
      return 3;
    case gx::VtxComponentSize::NormalNbt:
    case gx::VtxComponentSize::NormalNbt3:
    default:
      return 0;
    }
  } else if (attr == gx::VtxAttribute::Color0 || attr == gx::VtxAttribute::Color1) {
    switch (compSize) {
    case gx::VtxComponentSize::ColorRgb:
      return 3;
    case gx::VtxComponentSize::ColorRgba:
      return 4;
    default:
      return 0;
    }
  } else if (attr >= gx::VtxAttribute::TexCoord0 && attr <= gx::VtxAttribute::TexCoord7) {
    switch (compSize) {
    case gx::VtxComponentSize::TexCoordS:
      return 1;
    case gx::VtxComponentSize::TexCoordSt:
      return 2;
    default:
      return 0;
    }
  }

  return 0;
}

f32 ReadVertexAttributeComponent(athena::io::IStreamReader& inStream,
                                 gx::VtxComponentType compType) {
  switch (compType) {
  case gx::VtxComponentType::Uint8:
    return static_cast< f32 >(inStream.readUByte());
  case gx::VtxComponentType::Int8:
    return static_cast< f32 >(inStream.readByte());
  case gx::VtxComponentType::Uint16:
    return static_cast< f32 >(inStream.readUint16());
  case gx::VtxComponentType::Int16:
    return static_cast< f32 >(inStream.readInt16());
  case gx::VtxComponentType::Float:
    return inStream.readFloat();
  default:
    return 0.0f;
  }
}

f32 ReadVertexAttributeColorComponent(athena::io::IStreamReader& inStream,
                                      gx::VtxComponentType compType) {
  switch (compType) {
  case gx::VtxComponentType::Rgb565:
    return 0.0f;
  case gx::VtxComponentType::Rgb8:
  case gx::VtxComponentType::Rgbx8:
  case gx::VtxComponentType::Rgba8:
    return static_cast< f32 >(inStream.readByte()) / 255.0f;
  case gx::VtxComponentType::Rgba4:
    return 0.0f;
  case gx::VtxComponentType::Rgba6:
    return 0.0f;
  default:
    return 0.0f;
  }
}
} // namespace

binary::ChirpBinaryVat::ChirpBinaryVat() : ChirpBinarySection() {}

binary::ChirpBinaryVat::ChirpBinaryVat(u32 size, u32 elementCount)
: ChirpBinarySection(ChirpSectionType::Vertex, size, elementCount) {}

binary::ChirpBinaryVat::ChirpBinaryVat(u64 offset, u32 size, u32 elementCount)
: ChirpBinarySection(ChirpSectionType::Vertex, offset, size, elementCount) {}

bool binary::ChirpBinaryVat::Import(athena::io::IStreamReader& inStream) {
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

bool binary::ChirpBinaryVat::Export(athena::io::IStreamWriter& outStream) { return true; }

void binary::ChirpBinaryVat::ReadVertexAttributeData(athena::io::IStreamReader& inStream, u32 count,
                                                     ChirpVertexAttribute* attribute) {
  if (!count || !attribute) {
    return;
  }

  gx::VtxAttribute attr = attribute->GetAttribute();
  gx::VtxComponentType compType = attribute->GetComponentType();
  f32 scaleFactor = std::powf(0.5f, attribute->GetExponent());

  u32 compSize = GetComponentCount(attr, attribute->GetComponentSize());

  std::vector< math::ChirpVector4 >& attrData = attribute->GetData();
  attrData.resize(count);

  for (u32 i = 0; i < count; i++) {
    math::ChirpVector4 dataVec;

    for (u32 j = 0; j < compSize; j++) {
      if (attr == gx::VtxAttribute::Color0 || attr == gx::VtxAttribute::Color1) {
        dataVec[j] = ReadVertexAttributeColorComponent(inStream, compType);
      } else {
        dataVec[j] = ReadVertexAttributeComponent(inStream, compType) * scaleFactor;
      }
    }

    attrData[i] = dataVec;
  }
}

void binary::ChirpBinaryVat::WriteVertexAttributeData(athena::io::IStreamWriter& outStream,
                                                      u32 count, ChirpVertexAttribute* attribute) {}

binary::ChirpVertexAttribute* binary::ChirpBinaryVat::GetVertexAttribute(gx::VtxAttribute attr) {
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

binary::ChirpVertexAttribute*
binary::ChirpBinaryVat::CreateVertexAttribute(gx::VtxAttribute attr, gx::VtxComponentSize size,
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

  m_attributes[attr] = std::make_unique< ChirpVertexAttribute >(attr, size, type, exponent);
  return m_attributes[attr].get();
}

binary::ChirpVertexAttribute* binary::ChirpBinaryVat::CreateVertexAttribute(
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
