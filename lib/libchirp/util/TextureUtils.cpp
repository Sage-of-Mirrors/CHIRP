#include "libchirp/util/TextureUtils.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>

namespace chirp::binary::util {
// TODO: Are these correct?
void WritePaletteData(athena::io::IStreamWriter& outStream,
                      const std::vector< chirp::math::ChirpColor >& paletteData,
                      chirp::gx::TexFmt format) {
  for (const auto& color : paletteData) {
    switch (format) {
    case chirp::gx::TexFmt::I4: {
      // TODO: 2 colors per byte.
      break;
    }
    case chirp::gx::TexFmt::I8: {
      u32 intensity = (color.R() * 11) + (color.G() * 59) + (color.B() * 30);
      intensity /= 100;

      outStream.writeUByte(static_cast< u8 >(intensity));
      break;
    }
    case chirp::gx::TexFmt::IA4: {
      u32 intensity = (color.R() * 11) + (color.G() * 59) + (color.B() * 30);
      intensity /= 1700;
      intensity |= (color.A() & 0x0F) << 4;

      outStream.writeUByte(static_cast< u8 >(intensity));
      break;
    }
    case chirp::gx::TexFmt::IA8: {
      u32 intensity = (color.R() * 11) + (color.G() * 59) + (color.B() * 30);

      outStream.writeUByte(color.A());
      outStream.writeUByte(static_cast< u8 >(intensity / 100));

      break;
    }
    case chirp::gx::TexFmt::RGB565: {
      u16 bits{0};
      bits |= (color.R() & 0x1F) << 11;
      bits |= (color.G() & 0x3F) << 5;
      bits |= (color.B() & 0x1F);

      outStream.writeUint16(bits);
      break;
    }
    case chirp::gx::TexFmt::RGB5A3: {
      u16 bits{0};

      if (color.A()) {
        bits |= (color.A() & 0x07) << 12;
        bits |= (color.R() & 0x0F) << 8;
        bits |= (color.G() & 0x0F) << 4;
        bits |= (color.B() & 0x0F);
      } else {
        bits |= (color.R() & 0x1F) << 10;
        bits |= (color.G() & 0x1F) << 5;
        bits |= (color.B() & 0x1F);
      }

      outStream.writeUint16(bits);
      break;
    }
    case chirp::gx::TexFmt::RGBA8: {
      outStream.writeUByte(color.R());
      outStream.writeUByte(color.G());
      outStream.writeUByte(color.B());
      outStream.writeUByte(color.A());
      break;
    }
    }
  }
}
} // namespace chirp::binary::util