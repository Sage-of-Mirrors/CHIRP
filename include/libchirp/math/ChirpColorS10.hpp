#pragma once

#include "libchirp/ChirpCommon.hpp"

namespace chirp::math {
class ChirpColorS10 {
public:
  ChirpColorS10() : m_red(0), m_green(0), m_blue(0), m_alpha(0) {}

  ChirpColorS10(s16 red, s16 green, s16 blue, s16 alpha)
  : m_red(red), m_green(green), m_blue(blue), m_alpha(alpha) {}

  const s16& R() const { return m_red; }
  const s16& G() const { return m_green; }
  const s16& B() const { return m_blue; }
  const s16& A() const { return m_alpha; }

  s16& R() { return m_red; }
  s16& G() { return m_green; }
  s16& B() { return m_blue; }
  s16& A() { return m_alpha; }

private:
  s16 m_red;
  s16 m_green;
  s16 m_blue;
  s16 m_alpha;
};
} // namespace chirp::math