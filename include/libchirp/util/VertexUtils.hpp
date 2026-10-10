#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/GXEnum.hpp"
#include "libchirp/math/ChirpVector.hpp"

namespace athena::io {
class IStreamReader;
class IStreamWriter;
} // namespace athena::io

namespace chirp::binary {
class ChirpVertexAttribute;
} // namespace chirp::binary

namespace chirp::binary::util {
// Converts the given GX component size to the number of components it represents.
u32 GetComponentCount(gx::VtxAttribute attr, gx::VtxComponentSize compSize);

// Reads the given number of GX-formatted elements from the given stream to the given vector.
void ReadVertexAttributeGX(athena::io::IStreamReader& inStream, ChirpVertexAttribute* attribute,
                           u32 count);
// Writes the elements in the given vector to the given stream in GX format.
void WriteVertexAttributeGX(athena::io::IStreamWriter& outStream, ChirpVertexAttribute* attribute,
                            u32 count);
} // namespace chirp::binary::util
