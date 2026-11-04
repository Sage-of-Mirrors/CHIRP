#include "libchirp/section/ChirpBinaryTextures.hpp"

#include <athena/VectorWriter.hpp>

namespace {
// TODO: Are these correct?
void WritePaletteData(athena::io::IStreamWriter& outStream,
                      const chirp::gx::ChirpTexture::ChirpPaletteData& paletteData,
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
} // namespace

namespace chirp::binary {
ChirpBinaryTextures::ChirpBinaryTextures() : ChirpBinarySection() {}
ChirpBinaryTextures::ChirpBinaryTextures(u32 size, u32 elementCount, u16 versionMajor,
                                         u16 versionMinor)
: ChirpBinarySection(size, elementCount, versionMajor, versionMinor) {}
ChirpBinaryTextures::ChirpBinaryTextures(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                         u16 versionMinor)
: ChirpBinarySection(offset, size, elementCount, versionMajor, versionMinor) {}

bool ChirpBinaryTextures::Import(SectionReader& inStream) {
  m_imageDataOffset = inStream.readUint32();
  m_paletteDataOffset = inStream.readUint32();
  m_textureNamesOffset = inStream.readUint32();
  inStream.seekAlign16();

  for (uint32_t i = 0; i < m_elementCount; i++) {
    u16 nameSize = inStream.readUint16();
    u16 width = inStream.readUint16();
    u16 height = inStream.readUint16();
    u16 mipCount = inStream.readUint16();
    u16 paletteCount = inStream.readUint16();

    gx::TexFmt format = static_cast< gx::TexFmt >(inStream.readUByte());
    gx::TexWrapMode wrapS = static_cast< gx::TexWrapMode >(inStream.readUByte());
    gx::TexWrapMode wrapT = static_cast< gx::TexWrapMode >(inStream.readUByte());
    gx::TexFilter minFilter = static_cast< gx::TexFilter >(inStream.readUByte());
    gx::TexFilter magFilter = static_cast< gx::TexFilter >(inStream.readUByte());
    gx::Anisotropy aniso = static_cast< gx::Anisotropy >(inStream.readUByte());

    f32 minLod = inStream.readFloat();
    f32 maxLod = inStream.readFloat();
    f32 lodBias = inStream.readFloat();
    bool biasClamp = inStream.readBool();
    bool edgeLod = inStream.readBool();
    gx::TexFmt paletteFormat = static_cast< gx::TexFmt >(inStream.readUByte());
    inStream.seekAlign4();

    u32 nameOffset = inStream.readUint32();
    u32 paletteOffset = inStream.readUint32();
    u32 dataSize = inStream.readUint32();
    u32 dataOffset = inStream.readUint32();

    s64 nextTexPos = SeekToChunk(inStream, static_cast< u8 >(ChunkId::TextureNames), nameOffset);

    gx::ChirpTexture* newTex = CreateTexture(
        inStream.readString(nameSize), width, height, format, paletteFormat, mipCount, wrapS, wrapT,
        minFilter, magFilter, aniso, minLod, maxLod, lodBias, biasClamp, edgeLod);

    SeekToChunk(inStream, static_cast< u8 >(ChunkId::ImageData), dataOffset);

    auto& imgData = newTex->GetImageData();
    imgData.resize(dataSize);
    inStream.readUBytesToBuf(imgData.data(), dataSize);

    if (paletteCount) {
      SeekToChunk(inStream, static_cast< u8 >(ChunkId::PaletteData), paletteOffset);

      auto& paletteData = newTex->GetPaletteData();
      for (u32 c = 0; c < paletteCount; c++) {
        paletteData.emplace_back(inStream.readUByte(), inStream.readUByte(), inStream.readUByte(),
                                 inStream.readUByte());
      }
    }

    inStream.seek(nextTexPos, athena::SeekOrigin::Begin);
  }

  return true;
}

bool ChirpBinaryTextures::Export(SectionWriter& outStream) {
  // Write section header.
  outStream.writeUint32(FourCC("CTEX").toUint32());
  outStream.writeUint16(m_versionMajor);
  outStream.writeUint16(m_versionMinor);
  outStream.writeUint32(0); // Placeholder for section size.
  outStream.writeUint32(static_cast< u32 >(m_textures.size()));
  outStream.writeUint32(0); // Placeholder for image data offset.
  outStream.writeUint32(0); // Placeholder for palette data offset.
  outStream.writeUint32(0); // Placeholder for texture names offset.
  outStream.writeAlign16();

  athena::io::VectorWriter imageWriter;
  imageWriter.setEndian(athena::Endian::Little);
  athena::io::VectorWriter paletteWriter;
  paletteWriter.setEndian(athena::Endian::Little);
  athena::io::VectorWriter nameWriter;
  nameWriter.setEndian(athena::Endian::Little);

  for (auto& [name, tex] : m_textures) {
    outStream.writeUint16(static_cast< u16 >(name.size()) + 1);
    outStream.writeUint16(tex->GetWidth());
    outStream.writeUint16(tex->GetHeight());
    outStream.writeUint16(tex->GetMipCount());

    auto& paletteData = tex->GetPaletteData();
    outStream.writeUint16(static_cast< u16 >(paletteData.size()));

    outStream.writeUByte(static_cast< u8 >(tex->GetFormat()));
    outStream.writeUByte(static_cast< u8 >(tex->GetWrapS()));
    outStream.writeUByte(static_cast< u8 >(tex->GetWrapT()));
    outStream.writeUByte(static_cast< u8 >(tex->GetMinFilter()));
    outStream.writeUByte(static_cast< u8 >(tex->GetMagFilter()));
    outStream.writeUByte(static_cast< u8 >(tex->GetAnisotropy()));

    outStream.writeFloat(tex->GetMinLod());
    outStream.writeFloat(tex->GetMaxLod());
    outStream.writeFloat(tex->GetLodBias());
    outStream.writeBool(tex->GetBiasClamp());
    outStream.writeBool(tex->GetEdgeLod());
    outStream.writeUByte(static_cast< u8 >(tex->GetPaletteFormat()));
    outStream.writeAlign4();

    outStream.writeUint32(static_cast< u32 >(nameWriter.position()));
    nameWriter.writeString(name);

    if (paletteData.size()) {
      outStream.writeUint32(static_cast< u32 >(paletteWriter.position()));
      WritePaletteData(paletteWriter, paletteData, tex->GetPaletteFormat());
    } else {
      outStream.writeUint32(0);
    }

    auto& imageData = tex->GetImageData();
    outStream.writeUint32(static_cast< u32 >(imageData.size()));
    outStream.writeUint32(static_cast< u32 >(imageWriter.position()));
    imageWriter.writeUBytes(imageData.data(), imageData.size());
  }

  // Write image data.
  outStream.writeAlign16();
  outStream.writeOffset(kImageDataOffsetPos);
  outStream.writeUBytes(imageWriter.data().data(), imageWriter.length());

  // Write palette data, if necessary.
  if (paletteWriter.data().size()) {
    outStream.writeAlign16();
    outStream.writeOffset(kPaletteDataOffsetPos);
    outStream.writeUBytes(paletteWriter.data().data(), paletteWriter.length());
  }

  // Write name table.
  outStream.writeAlign16();
  outStream.writeOffset(kNameTableOffsetPos);
  outStream.writeUBytes(nameWriter.data().data(), nameWriter.length());

  outStream.writeAlign16();
  outStream.writeSectionSize();

  return true;
}

s64 ChirpBinaryTextures::SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset) {
  ChunkId chunk = static_cast< ChunkId >(chunkId);

  u32 chunkOffset{0};
  switch (chunk) {
  case ChunkId::ImageData:
    chunkOffset = m_imageDataOffset;
    break;
  case ChunkId::PaletteData:
    chunkOffset = m_paletteDataOffset;
    break;
  case ChunkId::TextureNames:
    chunkOffset = m_textureNamesOffset;
    break;
  default:
    break;
  }

  s64 curPos = sectionReader.position();
  sectionReader.seek(m_offset + chunkOffset + dataOffset, athena::SeekOrigin::Begin);

  return curPos;
}

std::unique_ptr< ChirpBinarySection > ChirpBinaryTextures::CreateTEXSection(u32 size,
                                                                            u32 elementCount,
                                                                            u16 versionMajor,
                                                                            u16 versionMinor) {
  return std::make_unique< ChirpBinaryTextures >(size, elementCount, versionMajor, versionMinor);
}
std::unique_ptr< ChirpBinarySection >
ChirpBinaryTextures::CreateTEXSectionWithOffset(u64 offset, u32 size, u32 elementCount,
                                                u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinaryTextures >(offset, size, elementCount, versionMajor,
                                                 versionMinor);
}
} // namespace chirp::binary
