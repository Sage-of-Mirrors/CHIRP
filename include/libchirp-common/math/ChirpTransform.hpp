#pragma once

#include "libchirp-common/ChirpCommon.hpp"
#include "libchirp-common/math/ChirpQuat.hpp"
#include "libchirp-common/math/ChirpVector.hpp"

namespace chirp::math {
class ChirpTransform {
public:
  ChirpTransform() {}
  ChirpTransform(const ChirpVector3& position, const ChirpQuaternion& rotation,
                 const ChirpVector3& scale)
  : m_position(position), m_rotation(rotation), m_scale(scale) {}

  const ChirpVector3& Position() const { return m_position; }
  const ChirpQuaternion& Rotation() const { return m_rotation; }
  const ChirpVector3& Scale() const { return m_scale; }
  ChirpVector3& Position() { return m_position; }
  ChirpQuaternion& Rotation() { return m_rotation; }
  ChirpVector3& Scale() { return m_scale; }

private:
  ChirpVector3 m_position;
  ChirpQuaternion m_rotation;
  ChirpVector3 m_scale;
};
} // namespace chirp::math
