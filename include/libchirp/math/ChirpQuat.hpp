#pragma once

#include "libchirp/ChirpCommon.hpp"

namespace chirp::math {
class ChirpQuaternion {
public:
  ChirpQuaternion() = default;
  ChirpQuaternion(f32 x, f32 y, f32 z, f32 w) : m_x(x), m_y(y), m_z(z), m_w(w) {}

  f32 X() const { return m_x; }
  f32 Y() const { return m_y; }
  f32 Z() const { return m_z; }
  f32 W() const { return m_w; }

  f32& X() { return m_x; }
  f32& Y() { return m_y; }
  f32& Z() { return m_z; }
  f32& W() { return m_w; }

private:
  f32 m_x{0.0f};
  f32 m_y{0.0f};
  f32 m_z{0.0f};
  f32 m_w{1.0f};
};
} // namespace chirp::math