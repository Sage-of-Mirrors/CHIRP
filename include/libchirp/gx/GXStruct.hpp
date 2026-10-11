#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/GXEnum.hpp"
#include "libchirp/math/ChirpColor.hpp"

namespace chirp::gx {
struct ColorCombiner {
  TevColorArg m_argA;
  TevColorArg m_argB;
  TevColorArg m_argC;
  TevColorArg m_argD;

  TevOp m_op;
  TevBias m_bias;
  TevScale m_scale;
  bool m_clamp;

  TevRegId m_outReg;
};

struct AlphaCombiner {
  TevAlphaArg m_argA;
  TevAlphaArg m_argB;
  TevAlphaArg m_argC;
  TevAlphaArg m_argD;

  TevOp m_op;
  TevBias m_bias;
  TevScale m_scale;
  bool m_clamp;

  TevRegId m_outReg;
};

struct TevOrder {
  TexCoordId m_texCoordId;
  TexMapId m_texMapId;
  ChannelId m_channelId;
};

struct TevStage {
  ColorCombiner m_colorCombiner;
  AlphaCombiner m_alphaCombiner;
  TevOrder m_tevOrder;
};

struct AlphaCompare {
  Compare m_compA{Compare::Never};
  u8 m_refA{0};
  AlphaOp m_op{AlphaOp::And};
  Compare m_compB{Compare::Never};
  u8 m_refB{0};
};

struct TevSwapMode {
  TevSwapSel m_rasterSel{TevSwapSel::SwapSel_0};
  TevSwapSel m_textureSel{TevSwapSel::SwapSel_0};
};

struct TevSwapModeTable {
  TevSwapSel m_swapSel;
  TevColorChan m_redChannel;
  TevColorChan m_blueChannel;
  TevColorChan m_greenChannel;
  TevColorChan m_alphaChannel;
};

struct BlendMode {
  BlendType m_type{BlendType::None};
  BlendFactor m_srcFactor{BlendFactor::Zero};
  BlendFactor m_dstFactor{BlendFactor::Zero};
  LogicOp m_op{LogicOp::Clear};
};

struct FogRangeAdj {
  bool m_enabled{false};
  u16 m_center{0};

  std::array< u16, 10 > m_adjTable{};
};

struct Fog {
  FogType m_type{FogType::None};
  f32 m_startZ{0.0f};
  f32 m_endZ{0.0f};
  f32 m_nearZ{0.0f};
  f32 m_farZ{0.0f};

  math::ChirpColor m_color{};
  FogRangeAdj m_rangeAdjust{};
};

struct ZMode {
  bool m_compareEnable;
  Compare m_func;
  bool m_updateEnable;
};

struct TevIndirectTexOrder {
  TexCoordId m_texCoordId;
  TexMapId m_texMapId;
};

struct TevIndirectScale {
  IndirectTexScale m_scaleS;
  IndirectTexScale m_ScaleT;
};

struct TevIndirectTexMtx {
  IndirectTexMtxId m_texMtxId;
  f32 m_matrix[2][3]; // TODO: Replace this with ChirpMatrix2x3 when it is created.
  s8 m_scaleExponent;
};

struct TevIndirectTile {
  u16 m_tileSizeS;
  u16 m_tileSizeT;
  u16 m_tileSpacingS;
  u16 m_tileSpacingT;
  IndirectTexFormat m_format;
  IndirectTexMtxId m_matrixSel;
  IndirectTexBiasSel m_biasSel;
  IndirectTexAlphaSel m_alphaSel;
};

struct TevIndirectStage {
  IndirectTexFormat m_format;
  IndirectTexBiasSel m_bias;
  IndirectTexMtxId m_texMtxId;
  IndirectTexWrap m_wrapS;
  IndirectTexWrap m_wrapT;
  bool m_addPrevious;
  bool m_indLod;
  IndirectTexAlphaSel m_alphaSel;

  TevIndirectTexOrder m_texOrder;
  TevIndirectScale m_texScale;
};
} // namespace chirp::gx
