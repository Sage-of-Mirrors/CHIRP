#pragma once

#include "libchirp/math/ChirpMatrix.hpp"

namespace chirp::math {
class ChirpVector2;
class ChirpVector3;

class ChirpMatrix2x3 : public ChirpMatrix< 2, 3 > {
public:
  ChirpMatrix2x3();
  ChirpMatrix2x3(const ChirpVector3& r0, const ChirpVector3& r1);

  ChirpVector3 GetRow(u32 idx) const;
  ChirpVector2 GetCol(u32 idx) const;
  void SetRow(const ChirpVector3& data, u32 idx);
  void SetCol(const ChirpVector2& data, u32 idx);
};
} // namespace chirp::math
