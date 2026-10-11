#include "libchirp/math/ChirpMatrix3x4.hpp"
#include "libchirp/math/ChirpQuat.hpp"
#include "libchirp/math/ChirpVector.hpp"

namespace chirp::math {
ChirpMatrix3x4::ChirpMatrix3x4() {
  m_data[0][0] = 1.0f;
  m_data[1][1] = 1.0f;
  m_data[2][2] = 1.0f;
}

ChirpMatrix3x4::ChirpMatrix3x4(const ChirpVector4& r0, const ChirpVector4& r1,
                               const ChirpVector4& r2) {
  SetRow(r0, 0);
  SetRow(r1, 1);
  SetRow(r2, 2);
}

ChirpVector4 ChirpMatrix3x4::GetRow(u32 idx) const {
  if (idx >= 3) {
    return ChirpVector4();
  }

  return ChirpVector4(m_data[idx][0], m_data[idx][1], m_data[idx][2], m_data[idx][3]);
}

ChirpVector3 ChirpMatrix3x4::GetCol(u32 idx) const {
  if (idx >= 4) {
    return ChirpVector3();
  }

  return ChirpVector3(m_data[0][idx], m_data[1][idx], m_data[2][idx]);
}

void ChirpMatrix3x4::SetRow(const ChirpVector4& data, u32 idx) {
  if (idx >= 3) {
    return;
  }

  m_data[idx][0] = data[0];
  m_data[idx][1] = data[1];
  m_data[idx][2] = data[2];
  m_data[idx][3] = data[3];
}

void ChirpMatrix3x4::SetCol(const ChirpVector3& data, u32 idx) {
  if (idx >= 4) {
    return;
  }

  m_data[0][idx] = data[0];
  m_data[1][idx] = data[1];
  m_data[2][idx] = data[2];
}

ChirpVector3 ChirpMatrix3x4::GetTranslation() const {
  return ChirpVector3(m_data[0][3], m_data[1][3], m_data[2][3]);
}

ChirpQuaternion ChirpMatrix3x4::GetRotation() const {
  // TODO: Matrix to quat calculation seems hard and may flip signs. Do we need it?
  return ChirpQuaternion();
}

ChirpVector3 ChirpMatrix3x4::GetScale() const {
  return ChirpVector3(m_data[0][0], m_data[1][1], m_data[2][2]);
}

ChirpMatrix3x4 ChirpMatrix3x4::MakeTranslation(const ChirpVector3& translation) {
  ChirpMatrix3x4 newTrans;

  newTrans[0][3] = translation.X();
  newTrans[1][3] = translation.Y();
  newTrans[2][3] = translation.Z();

  return newTrans;
}

ChirpMatrix3x4 ChirpMatrix3x4::MakeRotation(const ChirpQuaternion& rotation) {
  ChirpMatrix3x4 newRot;

  f32 xSq = rotation.X() * rotation.X();
  f32 ySq = rotation.Y() * rotation.Y();
  f32 zSq = rotation.Z() * rotation.Z();

  f32 xy = rotation.X() * rotation.Y();
  f32 xz = rotation.X() * rotation.Z();
  f32 xw = rotation.X() * rotation.W();

  f32 yz = rotation.Y() * rotation.Z();
  f32 yw = rotation.Y() * rotation.W();

  f32 zw = rotation.Z() * rotation.W();

  newRot[0][0] = 1 - (2 * (ySq + zSq));
  newRot[0][1] = 2 * (xy - zw);
  newRot[0][2] = 2 * (xz + yw);

  newRot[1][0] = 2 * (xy + zw);
  newRot[1][1] = 1 - (2 * (xSq + zSq));
  newRot[1][2] = 2 * (yz - xw);

  newRot[2][0] = 2 * (xz - yw);
  newRot[2][1] = 2 * (yz + xw);
  newRot[2][2] = 1 - (2 * (xSq + ySq));

  return newRot;
}

ChirpMatrix3x4 ChirpMatrix3x4::MakeScale(const ChirpVector3& scale) {
  ChirpMatrix3x4 newScale;

  newScale[0][0] = scale.X();
  newScale[1][1] = scale.Y();
  newScale[2][2] = scale.Z();

  return newScale;
}
} // namespace chirp::math