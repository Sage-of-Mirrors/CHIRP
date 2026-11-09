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

struct FogRangeAdjust {
  bool m_enabled;
  u16 m_center;

  u16 m_rangeValues[10];
};

struct Fog {
  gx::FogType m_type;

  f32 m_startZ;
  f32 m_endZ;

  f32 m_nearZ;
  f32 m_farZ;

  math::ChirpColor m_color;

  FogRangeAdjust m_rangeAdust;
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

  bool GetColorWritesEnabled() const { return m_colorWritesEnabled; }
  bool GetAlphaWritesEnabled() const { return m_alphaWritesEnabled; }
  bool GetDitheringEnabled() const { return m_ditheringEnabled; }

  void SetColorWritesEnabled(bool enabled) { m_colorWritesEnabled = enabled; }
  void SetAlphaWritesEnabled(bool enabled) { m_alphaWritesEnabled = enabled; }
  void SetDitheringEnabled(bool enabled) { m_ditheringEnabled = enabled; }

  AlphaCompare& GetAlphaCompare() { return m_alphaCompare; }
  BlendMode& GetBlendMode() { return m_blendMode; }
  Fog& GetFog() { return m_fog; }
  ZMode& GetZMode() { return m_zMode; }

private:
  bool m_colorWritesEnabled;
  bool m_alphaWritesEnabled;
  bool m_ditheringEnabled;

  AlphaCompare m_alphaCompare;
  BlendMode m_blendMode;
  Fog m_fog;
  ZMode m_zMode;
};
} // namespace chirp::gx::material
