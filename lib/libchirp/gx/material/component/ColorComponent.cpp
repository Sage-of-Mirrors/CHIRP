#include "libchirp/gx/material/component/ColorComponent.hpp"

#define STAGE_INDEX_CHECK(x)                                                                       \
  if (x >= 16) {                                                                                   \
    return nullptr;                                                                                \
  }

namespace chirp::gx::material {
math::ChirpColor* ColorComponent::GetTevRegColor(u32 stageIdx, TevRegId regId) {
  STAGE_INDEX_CHECK(stageIdx);
  if (regId >= TevRegId::Max) {
    return nullptr;
  }

  return &m_tevStageColors[stageIdx].m_tevRegColors[static_cast< u32 >(regId)];
}

math::ChirpColor* ColorComponent::GetTevKonstColor(u32 stageIdx, TevKColorId konstColorId) {
  STAGE_INDEX_CHECK(stageIdx);
  if (konstColorId >= TevKColorId::Max) {
    return nullptr;
  }

  return &m_tevStageColors[stageIdx].m_tevKonstColors[static_cast< u32 >(konstColorId)];
}

TevKColorSel* ColorComponent::GetTevKonstColorSel(u32 stageIdx) {
  STAGE_INDEX_CHECK(stageIdx);
  return &m_tevStageKonstSels[stageIdx].m_tevKonstColorSel;
}

TevKAlphaSel* ColorComponent::GetTevKonstAlphaSel(u32 stageIdx) {
  STAGE_INDEX_CHECK(stageIdx);
  return &m_tevStageKonstSels[stageIdx].m_tevKAlphaSel;
}

TevSwapTable* ColorComponent::GetTevSwapTable(u32 stageIdx, TevSwapSel swapId) {
  STAGE_INDEX_CHECK(stageIdx);
  if (swapId >= TevSwapSel::SwapSel_Max) {
    return nullptr;
  }

  return &m_tevStageSwapTables[stageIdx].m_tevSwapTables[static_cast< u32 >(swapId)];
}

TevSwapSel* ColorComponent::GetTevKonstSwapSel(u32 stageIdx) {
  STAGE_INDEX_CHECK(stageIdx);
  return &m_tevStageSwapTables[stageIdx].m_tevKonstSwapSel;
}

TevSwapSel* ColorComponent::GetTevTexSwapSel(u32 stageIdx) {
  STAGE_INDEX_CHECK(stageIdx);
  return &m_tevStageSwapTables[stageIdx].m_tevTexSwapSel;
}
} // namespace chirp::gx::material