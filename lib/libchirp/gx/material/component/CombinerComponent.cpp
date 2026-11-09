#include "libchirp/gx/material/component/CombinerComponent.hpp"

namespace chirp::gx::material {
TevStage* CombinerComponent::GetTevStage(u32 index) {
  return index < m_tevStages.size() ? &m_tevStages[index] : nullptr;
}

TevStage* CombinerComponent::AddTevStage() {
  if (m_tevStages.size() >= kMaxTevStageCount) {
    return nullptr;
  }

  m_tevStages.emplace_back();
  return &m_tevStages.back();
}

bool CombinerComponent::RemoveTevStage(u32 index) {
  if (m_tevStages.empty() || index >= static_cast< u32 >(m_tevStages.size())) {
    return false;
  }

  m_tevStages.erase(m_tevStages.begin() + index);
  return true;
}
} // namespace chirp::gx::material