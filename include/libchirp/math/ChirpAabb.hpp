#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/math/ChirpVector.hpp"

namespace chirp::math {
class ChirpAabb {
public:
  ChirpAabb() {}
  ChirpAabb(const ChirpVector3& min, const ChirpVector3& max) : m_minimum(min), m_maximum(max) {}

  const ChirpVector3& GetMinimum() const { return m_minimum; }
  const ChirpVector3& GetMaximum() const { return m_maximum; }
  ChirpVector3& GetMinimum() { return m_minimum; }
  ChirpVector3& GetMaximum() { return m_maximum; }

  f32 CalculateBoundingSphereInside();
  f32 CalcualteBoundingSphereOutside();

private:
  ChirpVector3 m_minimum;
  ChirpVector3 m_maximum;
};
} // namespace chirp::math
