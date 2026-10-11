#pragma once

#include "libchirp/math/ChirpMatrix.hpp"

namespace chirp::math {
class ChirpVector3;
class ChirpVector4;
class ChirpQuaternion;

class ChirpMatrix3x4 : public ChirpMatrix< 3, 4 > {
public:
  ChirpMatrix3x4();
  ChirpMatrix3x4(const ChirpVector4& r0, const ChirpVector4& r1, const ChirpVector4& r2);

  ChirpVector4 GetRow(u32 idx) const;
  ChirpVector3 GetCol(u32 idx) const;
  void SetRow(const ChirpVector4& data, u32 idx);
  void SetCol(const ChirpVector3& data, u32 idx);

  ChirpVector3 GetTranslation() const;
  ChirpQuaternion GetRotation() const;
  ChirpVector3 GetScale() const;

  static ChirpMatrix3x4 MakeTranslation(const ChirpVector3& translation);
  static ChirpMatrix3x4 MakeRotation(const ChirpQuaternion& rotation);
  static ChirpMatrix3x4 MakeScale(const ChirpVector3& scale);
};
} // namespace chirp::math
