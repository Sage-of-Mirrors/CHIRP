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

enum class VtxAttributeType : u8 { None, Direct, Index8, Index16 };

enum class Compare : u8 { Never, Less, Equal, LEqual, Greater, NEqual, GEqual, Always };

enum class AlphaOp : u8 { And, Or, Xor, Xnor, Max };

enum class BlendType : u8 { None, Blend, Logic, Subtract, Max };

enum class BlendFactor : u8 {
  Zero,
  One,
  SrcColor,
  InvSrcColor,
  SrcAlpha,
  InvSrcAlpha,
  DstAlpha,
  InvDstAlpha,
  DstColor = SrcColor,
  InvDstColor = InvSrcColor
};

enum class LogicOp : u8 {
  Clear,
  And,
  RevAnd,
  Copy,
  InvAnd,
  NoOp,
  Xor,
  Or,
  Nor,
  Equiv,
  Inv,
  RevOr,
  InvCopy,
  InvOr,
  Nand,
  Set
};

enum class TevRegId : u8 { TevPrev, TevReg_0, TevReg_1, TevReg_2, Max };

enum class DiffuseFn : u8 { None, Sign, Clamp };

enum class AttenuationFn : u8 { Spec, Spot, None };

enum class ColorSrc : u8 { Register, Vertex };

enum class TevColorArg : u8 {
  Color_Prev,
  Alpha_Prev,
  Color_0,
  Alpha_0,
  Color_1,
  Alpha_1,
  Color_2,
  Alpha_2,
  TexColor,
  TexAlpha,
  RasterColor,
  RasterAlpha,
  Color_One,
  Color_Half,
  KonstColor,
  Zero
};

enum class TevAlphaArg : u8 {
  Alpha_Prev,
  Alpha_0,
  Alpha_1,
  Alpha_2,
  TexAlpha,
  RasterAlpha,
  KonstAlpha,
  Zero
};

enum class TevOp : u8 {
  Add = 0,
  Sub = 1,
  CompR8Greater = 8,
  CompR8Equal = 9,
  CmpGR16Greater = 10,
  CompGR16Equal = 11,
  CompBGR24Greater = 12,
  CompBGR24Equal = 13,
  CompRGB8Greater = 14,
  CompRGB8Equal = 15,
  CompA8Greater = CompRGB8Greater,
  CompA8Equal = CompRGB8Equal
};

enum class TevBias : u8 { Bias_0, Bias_AddHalf, Bias_SubHalf, Bias_Max };

enum class TevScale : u8 { Scale_1, Scale_2, Scale_4, Scale_1_2, Scale_Max };

enum class TevKColorSel : u8 {
  KColorSel_8_8 = 0x00,
  KColorSel_7_8 = 0x01,
  KColorSel_6_8 = 0x02,
  KColorSel_5_8 = 0x03,
  KColorSel_4_8 = 0x04,
  KColorSel_3_8 = 0x05,
  KColorSel_2_8 = 0x06,
  KColorSel_1_8 = 0x07,
  KColorSel_1 = KColorSel_8_8,
  KColorSel_3_4 = KColorSel_6_8,
  KColorSel_1_2 = KColorSel_4_8,
  KColorSel_1_4 = KColorSel_2_8,
  KColorSel_K0 = 0x0C,
  KColorSel_K1 = 0x0D,
  KColorSel_K2 = 0x0E,
  KColorSel_K3 = 0x0F,
  KColorSel_K0_R = 0x10,
  KColorSel_K1_R = 0x11,
  KColorSel_K2_R = 0x12,
  KColorSel_K3_R = 0x13,
  KColorSel_K0_G = 0x14,
  KColorSel_K1_G = 0x15,
  KColorSel_K2_G = 0x16,
  KColorSel_K3_G = 0x17,
  KColorSel_K0_B = 0x18,
  KColorSel_K1_B = 0x19,
  KColorSel_K2_B = 0x1A,
  KColorSel_K3_B = 0x1B,
  KColorSel_K0_A = 0x1C,
  KColorSel_K1_A = 0x1D,
  KColorSel_K2_A = 0x1E,
  KColorSel_K3_A = 0x1F
};

enum class TevKAlphaSel : u8 {
  KAlphaSel_8_8 = 0x00,
  KAlphaSel_7_8 = 0x01,
  KAlphaSel_6_8 = 0x02,
  KAlphaSel_5_8 = 0x03,
  KAlphaSel_4_8 = 0x04,
  KAlphaSel_3_8 = 0x05,
  KAlphaSel_2_8 = 0x06,
  KAlphaSel_1_8 = 0x07,
  KAlphaSel_1 = KAlphaSel_8_8,
  KAlphaSel_3_4 = KAlphaSel_6_8,
  KAlphaSel_1_2 = KAlphaSel_4_8,
  KAlphaSel_1_4 = KAlphaSel_2_8,
  KAlphaSel_K0_R = 0x10,
  KAlphaSel_K1_R = 0x11,
  KAlphaSel_K2_R = 0x12,
  KAlphaSel_K3_R = 0x13,
  KAlphaSel_K0_G = 0x14,
  KAlphaSel_K1_G = 0x15,
  KAlphaSel_K2_G = 0x16,
  KAlphaSel_K3_G = 0x17,
  KAlphaSel_K0_B = 0x18,
  KAlphaSel_K1_B = 0x19,
  KAlphaSel_K2_B = 0x1A,
  KAlphaSel_K3_B = 0x1B,
  KAlphaSel_K0_A = 0x1C,
  KAlphaSel_K1_A = 0x1D,
  KAlphaSel_K2_A = 0x1E,
  KAlphaSel_K3_A = 0x1F
};

enum class TevKColorId : u8 { KColor_0 = 0, KColor_1, KColor_2, KColor_3, Max };

enum ZTexOp : u8 { Disable, Add, Replace, Max };

enum class IndirectTexFormat : u8 { Format_8, Format_5, Format_4, Format_3, Format_Max };

enum class IndirectTexBiasSel : u8 {
  BiasSelNone,
  BiasSelS,
  BiasSelT,
  BiasSelST,
  BiasSelU,
  BiasSelSU,
  BiasSelTU,
  BiasSelSTU,
  BiasSelMax
};

enum class IndirectTexAlphaSel : u8 { AlphaSelOff, AlphaSelS, AlphaSelT, AlphaSelU, AlphaSelMax };

enum class IndirectTexMtxId : u8 {
  MtxId_Off,
  MtxId_0,
  MtxId_1,
  MtxId_2,
  MtxId_S0 = 5,
  MtxId_S1,
  MtxId_S2,
  MtxId_T0 = 9,
  MtxId_T1,
  MtxId_T2
};

enum class IndirectTexWrap : u8 {
  Wrap_Off,
  Wrap_256,
  Wrap_128,
  Wrap_64,
  Wrap_32,
  Wrap_16,
  Wrap_0,
  Wrap_Max
};

enum class IndirectTexStageId : u8 { Stage_0, Stage_1, Stage_2, Stage_3, Stage_Max };

enum class IndirectTexScale : u8 {
  Scale_1,
  Scale_2,
  Scale_4,
  Scale_8,
  Scale_16,
  Scale_32,
  Scale_64,
  Scale_128,
  Scale_256,
  Scale_Max
};

enum class SpotFn : u8 { Off, Flat, Cos, Cos_2, Sharp, Ring_1, Ring_2 };

enum class DistAttnFn : u8 { Off, Gentle, Medium, Steep };

enum class CullMode : u8 { None, Front, Back, All };

enum class TevSwapSel : u8 { SwapSel_0 = 0, SwapSel_1, SwapSel_2, SwapSel_3, SwapSel_Max };

enum class TevColorChan : u8 { Red = 0, Green, Blue, Alpha };

enum class FogType : u8 {
  None = 0,
  PerspLinear = 2,
  PerspExponential = 4,
  PerspExponential_2 = 5,
  PerspRevExponential = 6,
  PerspRevExponential_2 = 7,
  OrthoLinear = 10,
  OrthoExponential = 12,
  OrthoExponential_2 = 13,
  OrthoRevExponential = 14,
  OrthoRevExponential_2 = 15,
  Linear = PerspLinear,
  Exponential = PerspExponential,
  Exponential_2 = PerspExponential_2,
  RevExponential = PerspRevExponential,
  RevExponential_2 = PerspRevExponential_2
};

enum class LightId : u16 {
  Light_0 = 0x001,
  Light_1 = 0x002,
  Light_2 = 0x004,
  Light_3 = 0x008,
  Light_4 = 0x010,
  Light_5 = 0x020,
  Light_6 = 0x040,
  Light_7 = 0x080,
  Light_Max = 0x100,
  Light_Null = 0
};

enum class TexOffset : u8 {
  TexOffset_0,
  TexOffset_1_16,
  TexOffset_1_8,
  TexOffset_1_4,
  TexOffset_1_2,
  TexOffset_1,
  TexOffset_Max
};

enum class TexMtx : u8 {
  TexMtx_0 = 30,
  TexMtx_1 = 33,
  TexMtx_2 = 36,
  TexMtx_3 = 39,
  TexMtx_4 = 42,
  TexMtx_5 = 45,
  TexMtx_6 = 48,
  TexMtx_7 = 51,
  TexMtx_8 = 54,
  TexMtx_9 = 57,
  TexMtx_Identity = 60
};

enum class ChannelId : u8 {
  Color_0,
  Color_1,
  Alpha_0,
  Alpha_1,
  ColorAlpha_0,
  ColorAlpha_1,
  ColorZero,
  AlphaBump,
  AlphaBumpN,
  ColorNull = 0xFF
};

enum class TexGenSrc : u8 {
  Position,
  Normal,
  Binormal,
  Tangent,
  Tex_0,
  Tex_1,
  Tex_2,
  Tex_3,
  Tex_4,
  Tex_5,
  Tex_6,
  Tex_7,
  TexCoord_0,
  TexCoord_1,
  TexCoord_2,
  TexCoord_3,
  TexCoord_4,
  TexCoord_5,
  TexCoord_6,
  Color_0,
  Color_1,
  Max
};

enum class TevMode : u8 { Modulate, Decal, Blend, Replace, PassColor };

enum TexMtxType : u8 { Mtx_3_4, Mtx_2_4 };

enum class TexGenType : u8 {
  Mtx_3_4,
  Mtx_2_4,
  Bump_0,
  Bump_1,
  Bump_2,
  Bump_3,
  Bump_4,
  Bump_5,
  Bump_6,
  Bump_7,
  Srtg
};

enum class TexMapId : u16 {
  TexMap_0,
  TexMap_1,
  TexMap_2,
  TexMap_3,
  TexMap_4,
  TexMap_5,
  TexMap_6,
  TexMap_7,
  TexMap_Max,
  TexMap_Null = 0xFF,
  TexMap_Disable = 0x100
};

enum class TexCoordId : u8 {
  TexCoord_0,
  TexCoord_1,
  TexCoord_2,
  TexCoord_3,
  TexCoord_4,
  TexCoord_5,
  TexCoord_6,
  TexCoord_7,
  TexCoord_Max,
  TexCoord_Null = 0xFF
};
} // namespace chirp::gx
