#pragma once

#include "libchirp/math/ChirpMatrix.hpp"

namespace chirp::math {
class ChirpVector3;
class ChirpVector4;
class ChirpQuaternion;

class ChirpMatrix4x4 : public ChirpMatrix< 4, 4 > {
public:
  ChirpMatrix4x4();
  ChirpMatrix4x4(const ChirpVector4& r0, const ChirpVector4& r1, const ChirpVector4& r2,
                 const ChirpVector4& r3);

  ChirpVector4 GetRow(u32 idx) const;
  ChirpVector4 GetCol(u32 idx) const;
  void SetRow(const ChirpVector4& data, u32 idx);
  void SetCol(const ChirpVector4& data, u32 idx);

  ChirpVector3 GetTranslation() const;
  ChirpQuaternion GetRotation() const;
  ChirpVector3 GetScale() const;

  static ChirpMatrix4x4 MakeTranslation(const ChirpVector3& translation);
  static ChirpMatrix4x4 MakeRotation(const ChirpQuaternion& rotation);
  static ChirpMatrix4x4 MakeScale(const ChirpVector3& scale);
};
} // namespace chirp::math
