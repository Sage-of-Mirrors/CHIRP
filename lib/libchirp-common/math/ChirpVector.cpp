#include "libchirp-common/math/ChirpVector.hpp"

chirp::math::ChirpVector2::operator ChirpVector3() const { return ChirpVector3(m_x, m_y, 0.0f); }

chirp::math::ChirpVector2::operator ChirpVector4() const {
  return ChirpVector4(m_x, m_y, 0.0f, 1.0f);
}

chirp::math::ChirpVector3::operator ChirpVector2() const { return ChirpVector2(m_x, m_y); }

chirp::math::ChirpVector3::operator ChirpVector4() const {
  return ChirpVector4(m_x, m_y, m_z, 1.0f);
}

chirp::math::ChirpVector4::operator ChirpVector2() const { return ChirpVector2(m_x, m_y); }

chirp::math::ChirpVector4::operator ChirpVector3() const { return ChirpVector3(m_x, m_y, m_z); }