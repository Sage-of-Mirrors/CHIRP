#include "libchirp/section/ChirpBinarySection.hpp"

#include "libchirp/math/ChirpAabb.hpp"
#include "libchirp/math/ChirpQuat.hpp"
#include "libchirp/math/ChirpTransform.hpp"
#include "libchirp/math/ChirpVector.hpp"

namespace chirp::binary {
void SectionWriter::writeVector3(const math::ChirpVector3& vec3) {
  writeFloat(vec3.X());
  writeFloat(vec3.Y());
  writeFloat(vec3.Z());
}

void SectionWriter::writeVector4(const math::ChirpVector4& vec4) {
  writeFloat(vec4.X());
  writeFloat(vec4.Y());
  writeFloat(vec4.Z());
  writeFloat(vec4.W());
}

void SectionWriter::writeQuaternion(const math::ChirpQuaternion& quat) {
  writeFloat(quat.X());
  writeFloat(quat.Y());
  writeFloat(quat.Z());
  writeFloat(quat.W());
}

void SectionWriter::writeTransform(const math::ChirpTransform& transform) {
  writeVector3(transform.Position());
  writeQuaternion(transform.Rotation());
  writeVector3(transform.Scale());
}

void SectionWriter::writeAabb(const math::ChirpAabb& aabb) {
  writeVector3(aabb.GetMinimum());
  writeVector3(aabb.GetMaximum());
}
} // namespace chirp::binary