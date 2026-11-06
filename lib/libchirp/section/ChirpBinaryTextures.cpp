#include "libchirp/section/ChirpBinaryTextures.hpp"

#include <athena/VectorWriter.hpp>

namespace {
struct TextureEntry {
  // u16 size values.
  /*0x00*/ u16 m_nameSize;
  /*0x02*/ u16 m_width;
  /*0x04*/ u16 m_height;
  /*0x06*/ u16 m_paletteColorCount; // ChirpBinary handles palette data as a single block of bytes,
                                    // but we maintain this value for others to use.
  /*0x08*/ u16 m_mipCount;          // MIP dimensions calculated by (dim / 2^mipIndex).

  // u8 config values.
  /*0x0A*/ chirp::gx::TexFmt m_imageFormat;
  /*0x0B*/ chirp::gx::PaletteFmt m_paletteFormat;
  /*0x0C*/ chirp::gx::TexWrapMode m_wrapS;
  /*0x0D*/ chirp::gx::TexWrapMode m_wrapT;
  /*0x0E*/ chirp::gx::TexFilter m_minFilter;
  /*0x0F*/ chirp::gx::TexFilter m_magFilter;
  /*0x10*/ chirp::gx::Anisotropy m_anisotropy;

  // u8 padding for alignment.
  /*0x11*/ u8 m_padding0;

  // LOD values.
  /*0x12*/ bool m_lodBiasClamp;
  /*0x13*/ bool m_edgeLod;
  /*0x14*/ f32 m_minLod;  // Range is [0.0..10.0].
  /*0x18*/ f32 m_maxLod;  // Range is [0.0..10.0].
  /*0x1C*/ f32 m_lodBias; // Range is [-4.0..4).

  // u32 size and offset values.
  /*0x20*/ u32 m_nameOffset;
  /*0x24*/ u32 m_imageDataSize;
  /*0x28*/ u32 m_imageDataOffset;
  /*0x2C*/ u32 m_paletteDataSize;
  /*0x30*/ u32 m_paletteDataOffset;
};

void ReadTextureEntry(athena::io::IStreamReader& inStream, TextureEntry& outEntry) {
  outEntry.m_nameSize = inStream.readUint16();
  outEntry.m_width = inStream.readUint16();
  outEntry.m_height = inStream.readUint16();
  outEntry.m_paletteColorCount = inStream.readUint16();
  outEntry.m_mipCount = inStream.readUint16();

  outEntry.m_imageFormat = static_cast< chirp::gx::TexFmt >(inStream.readUByte());
  outEntry.m_paletteFormat = static_cast< chirp::gx::PaletteFmt >(inStream.readUByte());
  outEntry.m_wrapS = static_cast< chirp::gx::TexWrapMode >(inStream.readUByte());
  outEntry.m_wrapT = static_cast< chirp::gx::TexWrapMode >(inStream.readUByte());
  outEntry.m_minFilter = static_cast< chirp::gx::TexFilter >(inStream.readUByte());
  outEntry.m_magFilter = static_cast< chirp::gx::TexFilter >(inStream.readUByte());
  outEntry.m_anisotropy = static_cast< chirp::gx::Anisotropy >(inStream.readUByte());

  outEntry.m_padding0 = inStream.readUByte();

  outEntry.m_lodBiasClamp = inStream.readBool();
  outEntry.m_edgeLod = inStream.readBool();
  outEntry.m_minLod = inStream.readFloat();
  outEntry.m_maxLod = inStream.readFloat();
  outEntry.m_lodBias = inStream.readFloat();

  outEntry.m_nameOffset = inStream.readUint32();
  outEntry.m_imageDataSize = inStream.readUint32();
  outEntry.m_imageDataOffset = inStream.readUint32();
  outEntry.m_paletteDataSize = inStream.readUint32();
  outEntry.m_paletteDataOffset = inStream.readUint32();
}

void WriteTextureEntry(athena::io::IStreamWriter& outStream, const TextureEntry& entry) {
  outStream.writeUint16(entry.m_nameSize);
  outStream.writeUint16(entry.m_width);
  outStream.writeUint16(entry.m_height);
  outStream.writeUint16(entry.m_paletteColorCount);
  outStream.writeUint16(entry.m_mipCount);

  outStream.writeUByte(static_cast< u8 >(entry.m_imageFormat));
  outStream.writeUByte(static_cast< u8 >(entry.m_paletteFormat));
  outStream.writeUByte(static_cast< u8 >(entry.m_wrapS));
  outStream.writeUByte(static_cast< u8 >(entry.m_wrapT));
  outStream.writeUByte(static_cast< u8 >(entry.m_minFilter));
  outStream.writeUByte(static_cast< u8 >(entry.m_magFilter));
  outStream.writeUByte(static_cast< u8 >(entry.m_anisotropy));

  outStream.writeUByte(0);

  outStream.writeBool(entry.m_lodBiasClamp);
  outStream.writeBool(entry.m_edgeLod);
  outStream.writeFloat(entry.m_minLod);
  outStream.writeFloat(entry.m_maxLod);
  outStream.writeFloat(entry.m_lodBias);

  outStream.writeUint32(entry.m_nameOffset);
  outStream.writeUint32(entry.m_imageDataSize);
  outStream.writeUint32(entry.m_imageDataOffset);
  outStream.writeUint32(entry.m_paletteDataSize);
  outStream.writeUint32(entry.m_paletteDataOffset);
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
    TextureEntry curEntry;
    ReadTextureEntry(inStream, curEntry);

    s64 nextTexPos =
        SeekToChunk(inStream, static_cast< u8 >(ChunkId::TextureNames), curEntry.m_nameOffset);

    gx::ChirpTexture* newTex = CreateTexture(
        inStream.readString(curEntry.m_nameSize), curEntry.m_width, curEntry.m_height,
        curEntry.m_imageFormat, curEntry.m_paletteFormat, curEntry.m_mipCount, curEntry.m_wrapS,
        curEntry.m_wrapT, curEntry.m_minFilter, curEntry.m_magFilter, curEntry.m_anisotropy,
        curEntry.m_minLod, curEntry.m_maxLod, curEntry.m_lodBias, curEntry.m_lodBiasClamp,
        curEntry.m_edgeLod);

    SeekToChunk(inStream, static_cast< u8 >(ChunkId::ImageData), curEntry.m_imageDataOffset);

    auto& imgData = newTex->GetImageData();
    imgData.resize(curEntry.m_imageDataSize);
    inStream.readUBytesToBuf(imgData.data(), curEntry.m_imageDataSize);

    // TODO: Retool everything here so that palettes are just chunks of memory like image data is.
    if (curEntry.m_paletteDataSize) {
      SeekToChunk(inStream, static_cast< u8 >(ChunkId::PaletteData), m_paletteDataOffset);

      auto& paletteData = newTex->GetPaletteData();
      paletteData.resize(curEntry.m_paletteDataSize);
      inStream.readUBytesToBuf(paletteData.data(), curEntry.m_paletteDataSize);
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
    TextureEntry curEntry;
    curEntry.m_nameSize = static_cast< u16 >(name.size()) + 1;
    curEntry.m_width = tex->GetWidth();
    curEntry.m_height = tex->GetHeight();
    curEntry.m_mipCount = tex->GetMipCount();
    curEntry.m_paletteColorCount = 0;

    curEntry.m_imageFormat = tex->GetFormat();
    curEntry.m_paletteFormat = tex->GetPaletteFormat();
    curEntry.m_wrapS = tex->GetWrapS();
    curEntry.m_wrapT = tex->GetWrapT();
    curEntry.m_minFilter = tex->GetMinFilter();
    curEntry.m_magFilter = tex->GetMagFilter();
    curEntry.m_anisotropy = tex->GetAnisotropy();

    curEntry.m_padding0 = 0;

    curEntry.m_lodBiasClamp = tex->GetBiasClamp();
    curEntry.m_edgeLod = tex->GetEdgeLod();
    curEntry.m_minLod = tex->GetMinLod();
    curEntry.m_maxLod = tex->GetMaxLod();
    curEntry.m_lodBias = tex->GetLodBias();

    curEntry.m_nameOffset = static_cast< u32 >(nameWriter.position());
    nameWriter.writeString(name);

    auto& imageData = tex->GetImageData();
    curEntry.m_imageDataSize = static_cast< u32 >(imageData.size());
    curEntry.m_imageDataOffset = static_cast< u32 >(imageWriter.position());
    imageWriter.writeUBytes(imageData.data(), imageData.size());

    auto& paletteData = tex->GetPaletteData();
    if (!paletteData.empty()) {
      curEntry.m_paletteDataSize = static_cast< u32 >(paletteData.size());
      curEntry.m_paletteDataOffset = static_cast< u32 >(paletteWriter.position());
      paletteWriter.writeUBytes(paletteData.data(), paletteData.size());
    } else {
      curEntry.m_paletteDataSize = 0;
      curEntry.m_paletteDataOffset = 0;
    }

    WriteTextureEntry(outStream, curEntry);
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
