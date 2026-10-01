#pragma once

#include "libchirp-common/ChirpCommon.hpp"

namespace chirp::gx {
enum class VtxAttribute : u8 {
  PositionMatrixIndex,
  Tex0MatrixIndex,
  Tex1MatrixIndex,
  Tex2MatrixIndex,
  Tex3MatrixIndex,
  Tex4MatrixIndex,
  Tex5MatrixIndex,
  Tex6MatrixIndex,
  Tex7MatrixIndex,
  Position,
  Normal,
  Color0,
  Color1,
  TexCoord0,
  TexCoord1,
  TexCoord2,
  TexCoord3,
  TexCoord4,
  TexCoord5,
  TexCoord6,
  TexCoord7,
  PositionMatrixArray,
  NormalMatrixArray,
  TexMatrixArray,
  LightArray,
  NormalBinormalTangent,
  VtxAttributeMax,
  Null = 0xFF,
};

enum class VtxComponentSize : u8 {
  PositionXy = 0,
  PositionXyz = 1,

  NormalXyz = 0,
  NormalNbt = 1,
  NormalNbt3 = 2,

  ColorRgb = 0,
  ColorRgba = 1,

  TexCoordS = 0,
  TexCoordSt = 1
};

enum class VtxComponentType : u8 {
  Uint8 = 0,
  Int8 = 1,
  Uint16 = 2,
  Int16 = 3,
  Float = 4,

  Rgb565 = 0,
  Rgb8 = 1,
  Rgbx8 = 2,
  Rgba4 = 3,
  Rgba6 = 4,
  Rgba8 = 5
};

enum class VtxAttributeType : u8 {
  None,
  Direct,
  Index8,
  Index16,
};
} // namespace chirp::gx
