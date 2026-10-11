#pragma once

#include "libchirp/ChirpCommon.hpp"

namespace chirp::math {
class ChirpColor {
public:
  ChirpColor() : m_red(0), m_green(0), m_blue(0), m_alpha(0) {}

  ChirpColor(u8 red, u8 green, u8 blue, u8 alpha)
  : m_red(red), m_green(green), m_blue(blue), m_alpha(alpha) {}

  ChirpColor(uint32_t color)
  : m_red((color & 0xFF000000) >> 24)
  , m_green((color & 0x00FF0000) >> 16)
  , m_blue((color & 0x0000FF00) >> 8)
  , m_alpha((color & 0x000000FF)) {}

  ChirpColor(f32 r, f32 g, f32 b, f32 a)
  : m_red(static_cast< u8 >(r * 255.0f))
  , m_green(static_cast< u8 >(g * 255.0f))
  , m_blue(static_cast< u8 >(b * 255.0f))
  , m_alpha(static_cast< u8 >(a * 255.0f)) {}

  const u8& R() const { return m_red; }
  const u8& G() const { return m_green; }
  const u8& B() const { return m_blue; }
  const u8& A() const { return m_alpha; }

  u8& R() { return m_red; }
  u8& G() { return m_green; }
  u8& B() { return m_blue; }
  u8& A() { return m_alpha; }

  f32 RAsFloat() const { return static_cast< f32 >(m_red) / 255.0f; }
  f32 GAsFloat() const { return static_cast< f32 >(m_green) / 255.0f; }
  f32 BAsFloat() const { return static_cast< f32 >(m_blue) / 255.0f; }
  f32 AAsFloat() const { return static_cast< f32 >(m_alpha) / 255.0f; }

  operator u32() { return (m_red << 24) | (m_green << 16) | (m_blue << 8) | (m_alpha); }

private:
  u8 m_red;
  u8 m_green;
  u8 m_blue;
  u8 m_alpha;
};
} // namespace chirp::math
