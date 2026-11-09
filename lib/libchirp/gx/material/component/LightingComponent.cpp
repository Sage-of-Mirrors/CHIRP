#include "libchirp/gx/material/component/LightingComponent.hpp"

namespace chirp::gx::material {
LightChannel* LightingComponent::GetLightChannel(ChannelId chanId) {
  auto itr = m_lightChannelMap.find(chanId);
  return itr != m_lightChannelMap.end() ? &itr->second : nullptr;
}

LightChannel* LightingComponent::AddChannel(ChannelId chanId) {
  auto result = m_lightChannelMap.try_emplace(chanId);
  return result.second ? &result.first->second : nullptr;
}

bool LightingComponent::RemoveChannel(ChannelId chanId) {
  auto itr = m_lightChannelMap.find(chanId);
  if (itr == m_lightChannelMap.end()) {
    return false;
  }

  m_lightChannelMap.erase(itr);
  return true;
}
} // namespace chirp::gx::material