#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/GXEnum.hpp"
#include "libchirp/math/ChirpColor.hpp"

namespace athena::io {
class IStreamReader;
class IStreamWriter;
} // namespace athena::io

namespace chirp::binary::util {
void WritePaletteData(athena::io::IStreamWriter& outStream,
                      const std::vector< chirp::math::ChirpColor >& paletteData,
                      chirp::gx::TexFmt format);
} // namespace chirp::binary::util
