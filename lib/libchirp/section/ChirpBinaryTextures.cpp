#include "libchirp/section/ChirpBinaryTextures.hpp"

#include <athena/IStreamWriter.hpp>

namespace chirp::binary {
ChirpBinaryTextures::ChirpBinaryTextures() : ChirpBinarySection() {}
ChirpBinaryTextures::ChirpBinaryTextures(u32 size, u32 elementCount, u16 versionMajor,
                                         u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Materials, size, elementCount, versionMajor, versionMinor) {}
ChirpBinaryTextures::ChirpBinaryTextures(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                         u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Materials, offset, size, elementCount, versionMajor,
                     versionMinor) {}

bool ChirpBinaryTextures::Import(SectionReader& inStream) {
  m_imageDataOffset = inStream.readUint32();
  m_textureNamesOffset = inStream.readUint32();
  inStream.seekAlign16();

  for (uint32_t i = 0; i < m_elementCount; i++) {
    u16 nameSize = inStream.readUint16();
    u16 width = inStream.readUint16();
    u16 height = inStream.readUint16();
    gx::TexFmt format = static_cast< gx::TexFmt >(inStream.readUByte());
    u8 mipCount = inStream.readUByte();

    gx::TexWrapMode wrapS = static_cast< gx::TexWrapMode >(inStream.readUByte());
    gx::TexWrapMode wrapT = static_cast< gx::TexWrapMode >(inStream.readUByte());
    gx::TexFilter minFilter = static_cast< gx::TexFilter >(inStream.readUByte());
    gx::TexFilter magFilter = static_cast< gx::TexFilter >(inStream.readUByte());
    f32 minLod = inStream.readFloat();
    f32 maxLod = inStream.readFloat();
    f32 lodBias = inStream.readFloat();

    u32 nameOffset = inStream.readUint32();
    u32 dataSize = inStream.readUint32();
    u32 dataOffset = inStream.readUint32();

    s64 nextTexPos = SeekToChunk(inStream, static_cast< u8 >(ChunkId::TextureNames), nameOffset);

    gx::ChirpTexture* newTex =
        CreateTexture(inStream.readString(nameSize), width, height, format, mipCount, wrapS, wrapT,
                      minFilter, magFilter, minLod, maxLod, lodBias);

    SeekToChunk(inStream, static_cast< u8 >(ChunkId::ImageData), dataOffset);

    auto& imgData = newTex->GetImageData();
    imgData.resize(dataSize);
    inStream.readUBytesToBuf(imgData.data(), dataSize);

    inStream.seek(nextTexPos, athena::SeekOrigin::Begin);
  }

  return true;
}

bool ChirpBinaryTextures::Export(athena::io::IStreamWriter& outStream) { return true; }

s64 ChirpBinaryTextures::SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset) {
  ChunkId chunk = static_cast< ChunkId >(chunkId);

  u32 chunkOffset{0};
  switch (chunk) {
  case ChunkId::ImageData:
    chunkOffset = m_imageDataOffset;
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
  return std::make_unique< ChirpBinaryTextures >(
      ChirpBinaryTextures(size, elementCount, versionMajor, versionMinor));
}
std::unique_ptr< ChirpBinarySection >
ChirpBinaryTextures::CreateTEXSectionWithOffset(u64 offset, u32 size, u32 elementCount,
                                                u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinaryTextures >(
      ChirpBinaryTextures(offset, size, elementCount, versionMajor, versionMinor));
}
} // namespace chirp::binary
