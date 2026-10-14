#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/Texture.hpp"
#include "libchirp/section/ChirpBinarySection.hpp"

namespace chirp::binary {
class ChirpBinaryTextures : public ChirpBinarySection {
public:
  ChirpBinaryTextures();
  ChirpBinaryTextures(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);
  ChirpBinaryTextures(u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);

  bool Import(SectionReader& inStream) override;
  bool Export(athena::io::IStreamWriter& outStream) override;

  bool HasTexture(std::string_view name) const { return m_textures.contains(std::string(name)); }

  gx::ChirpTexture* GetTexture(std::string_view name) const {
    std::string nameAsStr(name);
    return m_textures.contains(nameAsStr) ? m_textures.at(nameAsStr).get() : nullptr;
  }

  gx::ChirpTexture* CreateTexture(std::string_view name, u16 width, u16 height, gx::TexFmt format,
                                  gx::TexFmt paletteFormat = gx::TexFmt::RGBA8, u16 mipCount = 1,
                                  gx::TexWrapMode wrapS = gx::TexWrapMode::Clamp,
                                  gx::TexWrapMode wrapT = gx::TexWrapMode::Clamp,
                                  gx::TexFilter minFilter = gx::TexFilter::Linear,
                                  gx::TexFilter maxFilter = gx::TexFilter::Linear,
                                  gx::Anisotropy aniso = gx::Anisotropy::Aniso_1, f32 minLod = 0.0f,
                                  f32 maxLod = 0.0f, f32 lodBias = 0.0f, bool biasClamp = false,
                                  bool edgeLod = false) {
    std::string nameAsStr(name);
    if (m_textures.contains(nameAsStr)) {
      return nullptr;
    }

    m_textures[nameAsStr] = std::make_unique< gx::ChirpTexture >(
        name, width, height, format, paletteFormat, mipCount, wrapS, wrapT, minFilter, maxFilter,
        aniso, minLod, maxLod, lodBias, biasClamp, edgeLod);
    return m_textures[nameAsStr].get();
  }

  static std::unique_ptr< ChirpBinarySection > CreateTEXSection(u32 size, u32 elementCount,
                                                                u16 versionMajor, u16 versionMinor);
  static std::unique_ptr< ChirpBinarySection > CreateTEXSectionWithOffset(u64 offset, u32 size,
                                                                          u32 elementCount,
                                                                          u16 versionMajor,
                                                                          u16 versionMinor);

protected:
  s64 SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset = 0) override;

private:
  enum class ChunkId : u8 { ImageData, PaletteData, TextureNames };

  u32 m_imageDataOffset{0};
  u32 m_paletteDataOffset{0};
  u32 m_textureNamesOffset{0};

  std::unordered_map< std::string, std::unique_ptr< gx::ChirpTexture > > m_textures;
};
} // namespace chirp::binary
