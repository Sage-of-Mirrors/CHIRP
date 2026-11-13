#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/GXEnum.hpp"

#include <vector>

namespace chirp::gx::material {
constexpr u32 kMaxTevStageCount{16};

// Configures the calculation to perform to get the RGB components of the output color. Given color
// inputs A, B, C, and D, the equation for the Add and Sub operations is:
//
// resultColor = ((D +/- mix(A, B, C)) + bias) * scale
//
// while the equation for all other operations is:
//
// resultColor = D + (A cmp B ? C : 0)
//
// where "cmp" is a comparison operator, either > or == depending on the specific operation. The
// GR16 and BGR24 comparisons pack the relevant components into a single integer for comparison,
// while RGB8 compares each component of the color separately.
struct TevColorCombiner {
  TevColorArg m_colorArgA{TevColorArg::Zero};
  TevColorArg m_colorArgB{TevColorArg::Zero};
  TevColorArg m_colorArgC{TevColorArg::Zero};
  TevColorArg m_colorArgD{TevColorArg::Zero};

  TevOp m_colorOp{TevOp::Add};
  TevBias m_colorBias{TevBias::Bias_0};
  TevScale m_colorScale{TevScale::Scale_1};
  bool m_colorClamp{false};
  TevRegId m_colorOutReg{TevRegId::TevPrev};
};

// Configures the calculation to perform to get the alpha component of the output color.
// Uses the same equations as TevColorCombiner.
struct TevAlphaCombiner {
  TevAlphaArg m_alphaArgA{TevAlphaArg::Zero};
  TevAlphaArg m_alphaArgB{TevAlphaArg::Zero};
  TevAlphaArg m_alphaArgC{TevAlphaArg::Zero};
  TevAlphaArg m_alphaArgD{TevAlphaArg::Zero};

  TevOp m_alphaOp{TevOp::Add};
  TevBias m_alphaBias{TevBias::Bias_0};
  TevScale m_alphaScale{TevScale::Scale_1};
  bool m_alphaClamp{false};
  TevRegId m_alphaOutReg{TevRegId::TevPrev};
};

// Configures the samplers for textures and raster color. If a combiner input is TexColor or
// TexAlpha, the given texture map and texture coordinates are used to sample the texture. If a
// combiner input is RasterColor or RasterAlpha, the given color channel is sampled. Note that these
// are not mutually exclusive; a combiner can use either, both, or neither of the samplers.
struct TevOrder {
  TexCoordId m_texCoordId{TexCoordId::TexCoord_0};
  TexMapId m_texMapId{TexMapId::TexMap_Disable};

  ChannelId m_channelId{ChannelId::ColorNull};
};

// Represents a single color/alpha calculation in the TEV pipeline.
struct TevStage {
  TevColorCombiner m_colorCombiner;
  TevAlphaCombiner m_alphaCombiner;
  TevOrder m_tevOrder;
};

class CombinerComponent {
public:
  static constexpr u32 kMaxTevStageCount{16};

  CombinerComponent() {}

  u32 GetTevStageCount() const { return static_cast< u32 >(m_tevStages.size()); }
  TevStage* GetTevStage(u32 index);

  TevStage* AddTevStage();
  bool RemoveTevStage(u32 index);

  TevStage* operator[](u32 index) { return GetTevStage(index); }

private:
  std::vector< TevStage > m_tevStages;
};
} // namespace chirp::gx::material
