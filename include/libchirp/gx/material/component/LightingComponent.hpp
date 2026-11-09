#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/GXEnum.hpp"
#include "libchirp/math/ChirpColor.hpp"

#include <array>
#include <unordered_map>

namespace chirp::gx::material {
struct LightChannel {
  bool m_enabled{false};

  // Destination channel of the lighting calculations' output color.
  ChannelId m_channelId;

  // Source of the ambient color (color of the environment or "darkness", i.e. color without
  // lighting applied).
  ColorSrc m_ambientColorSrc;
  // Source of the material color ("base" color of the object, multiplied with the accumulated
  // lighting color).
  ColorSrc m_materialColorSrc;

  // Bitmask indicating which of the 8 light objects are enabled for this channel, ranging from bits
  // 0 to 7 (masks 0x00000001 to 0x00000080).
  u32 m_lightMask{0};

  // Calculation to use for diffuse lighting (lighting WITHOUT distance falloff).
  DiffuseFn m_diffuseFunc;
  // Calculation to use for attenuated lighting (lighting WITH distance falloff).
  AttenuationFn m_attenuationFunc;

  // Ambient colors to use when m_ambientColorSrc is Register. Otherwise, vertex colors are used.
  std::array< math::ChirpColor, 2 > m_defaultAmbientColors;
  // Material colors to use when m_materialColorSrc is set to Register. Otherwise, vertex colors are
  // used.
  std::array< math::ChirpColor, 2 > m_materialColors;
};

class LightingComponent {
public:
  LightingComponent() {}

  size_t GetChannelCount() const { return m_lightChannelMap.size(); }

  // Attempts to return the light channel with the given destination. Returns nullptr if no channel
  // with that destination exists.
  LightChannel* GetLightChannel(ChannelId chanId);

  // Attempts to add a light channel with the given destination and returns a pointer to the
  // newly-added channel if successful. Otherwise, returns nullptr.
  LightChannel* AddChannel(ChannelId chanId);
  // Attempts to remove the channel with the given destination. Returns whether the removal
  // succeeded.
  bool RemoveChannel(ChannelId chanId);

private:
  std::unordered_map< ChannelId, LightChannel > m_lightChannelMap;
};
} // namespace chirp::gx::material
