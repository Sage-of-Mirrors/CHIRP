#pragma once

#include "libchirp/ChirpCommon.hpp"

namespace chirp::math {
template < u32 n, u32 m >
class ChirpMatrix {
public:
  ChirpMatrix() = default;

  void Transpose() {
    for (u32 r = 0; r < m; r++) {
      for (u32 c = 0; c < n; c++) {
        if (r == c) {
          continue;
        }

        std::swap(m_data[r][c], m_data[c][r]);
      }
    }
  }

  const std::array< f32, m >& operator[](u32 row) const { return m_data[row]; }
  std::array< f32, m >& operator[](u32 row) { return m_data[row]; }

protected:
  std::array< std::array< f32, m >, n > m_data{};
};
} // namespace chirp::math
