#include "libchirp-binary/util/VertexUtils.hpp"
#include "libchirp-binary/section/ChirpBinaryVat.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>

#include <cmath>

namespace chirp::binary::util {
namespace {

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

void ReadVertexAttributeGX(athena::io::IStreamReader& inStream, ChirpVertexAttribute* attribute,
                           u32 count) {
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

void WriteVertexAttributeGX(athena::io::IStreamWriter& outStream, ChirpVertexAttribute* attribute,
                            u32 count) {}
} // namespace chirp::binary::util