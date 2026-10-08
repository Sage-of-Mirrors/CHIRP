#pragma once

#include "libchirp-common/ChirpCommon.hpp"

#include "libchirp-common/gx/GXEnum.hpp"
#include "libchirp-common/math/ChirpColor.hpp"

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
  Compare m_compA;
  u8 m_refA;
  AlphaOp m_op;
  Compare m_compB;
  u8 m_refB;
};

struct TevSwapMode {
  TevSwapSel m_rasterSel;
  TevSwapSel m_textureSel;
};

struct TevSwapModeTable {
  TevSwapSel m_swapSel;
  TevColorChan m_redChannel;
  TevColorChan m_blueChannel;
  TevColorChan m_greenChannel;
  TevColorChan m_alphaChannel;
};

struct BlendMode {
  BlendType m_type;
  BlendFactor m_srcFactor;
  BlendFactor m_dstFactor;
  LogicOp m_op;
};

struct FogRangeAdj {
  bool m_enabled;
  u16 m_center;

  std::array< u16, 10 > m_adjTable;
};

struct Fog {
  FogType m_type;
  f32 m_startZ;
  f32 m_endZ;
  f32 m_nearZ;
  f32 m_farZ;

  math::ChirpColor m_color;
  FogRangeAdj m_rangeAdjust;
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
  IndirectTexAlphaSel m_alphaSel
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
