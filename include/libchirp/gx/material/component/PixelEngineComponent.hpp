#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/GXStruct.hpp"
#include "libchirp/math/ChirpColor.hpp"

namespace chirp::gx::material {
struct AlphaCompare {
  gx::Compare m_compareA;
  u8 m_refA;

  gx::LogicOp m_operation;

  gx::Compare m_compareB;
  u8 m_refB;
};

struct BlendMode {
  gx::BlendMode m_blendType;

  gx::BlendFactor m_srcFactor;
  gx::BlendFactor m_dstFactor;

  gx::LogicOp m_operation;
};

struct Fog {
  gx::FogType m_type;

  f32 m_startZ;
  f32 m_endZ;

  f32 m_nearZ;
  f32 m_farZ;

  math::ChirpColor m_color;

  bool m_rangeAdjustEnabled;
  u16 m_rangeAdjustCenter;
  u16 m_rangeAdjustValues[10];
};

struct ZMode {
  bool m_enabled;
  gx::Compare m_compareFunc;
  bool m_allowUpdates;

  bool m_zCompLoc;
};

class PixelEngineComponent {
public:
  PixelEngineComponent() {}

  bool& GetColorWritesEnabled() { return m_colorWritesEnabled; }
  bool& GetAlphaWritesEnabled() { return m_alphaWritesEnabled; }
  bool& GetDitheringEnabled() { return m_ditheringEnabled; }
  gx::CullMode& GetCullMode() { return m_cullMode; }

  AlphaCompare& GetAlphaCompare() { return m_alphaCompare; }
  BlendMode& GetBlendMode() { return m_blendMode; }
  Fog& GetFog() { return m_fog; }
  ZMode& GetZMode() { return m_zMode; }

private:
  bool m_colorWritesEnabled;
  bool m_alphaWritesEnabled;
  bool m_ditheringEnabled;
  gx::CullMode m_cullMode;

  AlphaCompare m_alphaCompare;
  BlendMode m_blendMode;
  Fog m_fog;
  ZMode m_zMode;
};
} // namespace chirp::gx::material
