#pragma once

#include "libchirp-common/ChirpCommon.hpp"

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

private:
  u8 m_red;
  u8 m_green;
  u8 m_blue;
  u8 m_alpha;
};
} // namespace chirp::math
