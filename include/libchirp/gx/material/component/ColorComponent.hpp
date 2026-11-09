#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/GXEnum.hpp"
#include "libchirp/math/ChirpColor.hpp"

#include <array>

namespace chirp::gx::material {
// Colors that can be loaded into one of the 4 TEV registers or one of TEV's 4 "Konst" color slots.
struct TevColors {
  static constexpr u32 kTevKonstRegColorCount{4};

  std::array< math::ChirpColor, kTevKonstRegColorCount > m_tevRegColors;
  std::array< math::ChirpColor, kTevKonstRegColorCount > m_tevKonstColors;
};

// Configures what Konst color value is used when a combiner has Konst as an input. Can indicate
// either a constant (1, 1/8, 1/4, etc.) or one of the 4 "Konst" color slots.
struct TevKonstSels {
  TevKColorSel m_tevKonstColorSel;
  TevKAlphaSel m_tevKAlphaSel;
};

using TevSwapTable = std::array< TevColorChan, 4 >;

// Configures how Konst colors and sampled texture colors have their components swizzled.
struct TevSwapTables {
  static constexpr u32 kTevSwapTableCount{4};

  std::array< TevSwapTable, kTevSwapTableCount > m_tevSwapTables;

  TevSwapSel m_tevKonstSwapSel;
  TevSwapSel m_tevTexSwapSel;
};

// TEV material information related to register and Konst colors.
class ColorComponent {
public:
  static constexpr u32 kMaxTevColorsCount{16};
  static constexpr u32 kMaxTevKonstSelsCount{16};
  static constexpr u32 kMaxTevSwapTablesCount{16};

  ColorComponent() {}

  math::ChirpColor* GetTevRegColor(u32 stageIdx, TevRegId regId);
  math::ChirpColor* GetTevKonstColor(u32 stageIdx, TevKColorId konstColorId);

  TevKColorSel* GetTevKonstColorSel(u32 stageIdx);
  TevKAlphaSel* GetTevKonstAlphaSel(u32 stageIdx);

  TevSwapTable* GetTevSwapTable(u32 stageIdx, TevSwapSel swapId);

  TevSwapSel* GetTevKonstSwapSel(u32 stageIdx);
  TevSwapSel* GetTevTexSwapSel(u32 stageIdx);

private:
  std::array< TevColors, kMaxTevColorsCount > m_tevStageColors;
  std::array< TevKonstSels, kMaxTevKonstSelsCount > m_tevStageKonstSels;
  std::array< TevSwapTables, kMaxTevSwapTablesCount > m_tevStageSwapTables;
};
} // namespace chirp::gx::material
