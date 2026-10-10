#pragma once

#include "libchirp-binary/ChirpCommon.hpp"
#include "libchirp-binary/gx/GXEnum.hpp"
#include "libchirp-binary/gx/GXStruct.hpp"
#include "libchirp-binary/math/ChirpColor.hpp"

namespace chirp::gx {
class Material {
public:
  Material(std::string_view name) : m_name(name) {}

private:
  std::string m_name;

  u16 m_stageCount;
  std::array< TevStage, 16 > m_stages;

  u16 m_indirectStageCount;
  std::array< TevIndirectStage, 16 > m_indirectStages;

  bool m_colorUpdate;
  bool m_alphaUpdate;
  bool m_zCompLoc;
  bool m_dither;

  std::array< math::ChirpColor, 16 > m_tevColors;
  std::array< math::ChirpColor, 16 > m_konstColors;
  std::array< TevKColorSel, 16 > m_konstColorSels;
  std::array< TevKAlphaSel, 16 > m_konstAlphaSels;
  std::array< TevSwapMode, 16 > m_swapModes;

  AlphaCompare m_alphaCompare;
  BlendMode m_blendMode;
  Fog m_fog;
};
} // namespace chirp::gx
