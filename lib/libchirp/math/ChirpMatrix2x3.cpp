#include "libchirp/math/ChirpMatrix2x3.hpp"
#include "libchirp/math/ChirpVector.hpp"

namespace chirp::math {
ChirpMatrix2x3::ChirpMatrix2x3() {
  m_data[0][0] = 1.0f;
  m_data[1][1] = 1.0f;
}

ChirpMatrix2x3::ChirpMatrix2x3(const ChirpVector3& r0, const ChirpVector3& r1) {
  SetRow(r0, 0);
  SetRow(r1, 1);
}

ChirpVector3 ChirpMatrix2x3::GetRow(u32 idx) const {
  if (idx >= 2) {
    return ChirpVector3();
  }

  return ChirpVector3(m_data[idx][0], m_data[idx][1], m_data[idx][2]);
}

ChirpVector2 ChirpMatrix2x3::GetCol(u32 idx) const {
  if (idx >= 3) {
    return ChirpVector2();
  }

  return ChirpVector2(m_data[0][idx], m_data[1][idx]);
}

void ChirpMatrix2x3::SetRow(const ChirpVector3& data, u32 idx) {
  if (idx >= 2) {
    return;
  }

  m_data[idx][0] = data[0];
  m_data[idx][1] = data[1];
  m_data[idx][2] = data[2];
}

void ChirpMatrix2x3::SetCol(const ChirpVector2& data, u32 idx) {
  if (idx >= 3) {
    return;
  }

  m_data[0][idx] = data[0];
  m_data[1][idx] = data[1];
}
} // namespace chirp::math
