#pragma once

#include "libchirp/ChirpCommon.hpp"

namespace chirp::math {
class ChirpVector2;
class ChirpVector3;
class ChirpVector4;

class ChirpVector2 {
public:
  ChirpVector2() = default;
  ChirpVector2(f32 x, f32 y) : m_x(x), m_y(y) {}

  f32 X() const { return m_x; }
  f32 Y() const { return m_y; }

  f32& X() { return m_x; }
  f32& Y() { return m_y; }

  operator ChirpVector3() const;
  operator ChirpVector4() const;

  f32 operator[](u32 idx) const {
    if (idx == 0) {
      return m_x;
    } else if (idx == 1) {
      return m_y;
    }

    return 0.0f;
  }

  f32& operator[](u32 idx) {
    if (idx == 0) {
      return m_x;
    } else if (idx == 1) {
      return m_y;
    }

    // TODO: figure out what to do about this...
    return m_x;
  }

private:
  f32 m_x{0.0f};
  f32 m_y{0.0f};
};

class ChirpVector3 {
public:
  ChirpVector3() = default;
  ChirpVector3(f32 x, f32 y, f32 z) : m_x(x), m_y(y), m_z(z) {}

  f32 X() const { return m_x; }
  f32 Y() const { return m_y; }
  f32 Z() const { return m_z; }

  f32& X() { return m_x; }
  f32& Y() { return m_y; }
  f32& Z() { return m_z; }

  operator ChirpVector2() const;
  operator ChirpVector4() const;

  f32 operator[](u32 idx) const {
    if (idx == 0) {
      return m_x;
    } else if (idx == 1) {
      return m_y;
    } else if (idx == 2) {
      return m_z;
    }

    return 0.0f;
  }

  f32& operator[](u32 idx) {
    if (idx == 0) {
      return m_x;
    } else if (idx == 1) {
      return m_y;
    } else if (idx == 2) {
      return m_z;
    }

    // TODO: figure out what to do about this...
    return m_x;
  }

private:
  f32 m_x{0.0f};
  f32 m_y{0.0f};
  f32 m_z{0.0f};
};

class ChirpVector4 {
public:
  ChirpVector4() = default;
  ChirpVector4(f32 x, f32 y, f32 z, f32 w) : m_x(x), m_y(y), m_z(z), m_w(w) {}

  f32 X() const { return m_x; }
  f32 Y() const { return m_y; }
  f32 Z() const { return m_z; }
  f32 W() const { return m_w; }

  f32& X() { return m_x; }
  f32& Y() { return m_y; }
  f32& Z() { return m_z; }
  f32& W() { return m_w; }

  operator ChirpVector2() const;
  operator ChirpVector3() const;

  f32 operator[](u32 idx) const {
    if (idx == 0) {
      return m_x;
    } else if (idx == 1) {
      return m_y;
    } else if (idx == 2) {
      return m_z;
    } else if (idx == 3) {
      return m_w;
    }

    return 0.0f;
  }

  f32& operator[](u32 idx) {
    if (idx == 0) {
      return m_x;
    } else if (idx == 1) {
      return m_y;
    } else if (idx == 2) {
      return m_z;
    } else if (idx == 3) {
      return m_w;
    }

    // TODO: figure out what to do about this...
    return m_x;
  }

private:
  f32 m_x{0.0f};
  f32 m_y{0.0f};
  f32 m_z{0.0f};
  f32 m_w{0.0f};
};
} // namespace chirp::math
