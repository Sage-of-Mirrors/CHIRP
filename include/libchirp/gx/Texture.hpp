#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/GXEnum.hpp"
#include "libchirp/math/ChirpColor.hpp"

#include <vector>

namespace chirp::gx {
class ChirpTexture {
public:
  using ChirpPaletteData = std::vector< math::ChirpColor >;
  using ChirpImageData = std::vector< u8 >;

  ChirpTexture(std::string_view name, u16 width, u16 height, gx::TexFmt format,
               gx::TexFmt paletteFormat, u16 mipCount, gx::TexWrapMode wrapS, gx::TexWrapMode wrapT,
               gx::TexFilter minFilter, gx::TexFilter magFilter, gx::Anisotropy aniso, f32 minLod,
               f32 maxLod, f32 lodBias, bool biasClamp, bool edgeLod)
  : m_name(name)
  , m_width(width)
  , m_height(height)
  , m_format(format)
  , m_paletteFormat(paletteFormat)
  , m_mipCount(mipCount)
  , m_wrapS(wrapS)
  , m_wrapT(wrapT)
  , m_minFilter(minFilter)
  , m_magFilter(magFilter)
  , m_anisotropy(aniso)
  , m_minLod(minLod)
  , m_maxLod(maxLod)
  , m_lodBias(lodBias)
  , m_biasClamp(biasClamp)
  , m_edgeLod(edgeLod) {}

  ChirpTexture(std::string_view name, u16 width, u16 height, gx::TexFmt format)
  : ChirpTexture(name, width, height, format, gx::TexFmt::RGBA8, 1, gx::TexWrapMode::Clamp,
                 gx::TexWrapMode::Clamp, gx::TexFilter::Linear, gx::TexFilter::Linear,
                 gx::Anisotropy::Aniso_1, 0.0f, 1.0f, 0.0f, false, false) {}

  std::string GetName() const { return m_name; }
  u16 GetWidth() const { return m_width; }
  u16 GetHeight() const { return m_height; }
  u16 GetMipCount() const { return m_mipCount; }

  gx::TexFmt GetFormat() const { return m_format; }
  gx::TexFmt GetPaletteFormat() const { return m_paletteFormat; }
  gx::TexWrapMode GetWrapS() const { return m_wrapS; }
  gx::TexWrapMode GetWrapT() const { return m_wrapT; }
  gx::TexFilter GetMinFilter() const { return m_minFilter; }
  gx::TexFilter GetMagFilter() const { return m_magFilter; }
  gx::Anisotropy GetAnisotropy() const { return m_anisotropy; }

  f32 GetMinLod() const { return m_minLod; }
  f32 GetMaxLod() const { return m_maxLod; }
  f32 GetLodBias() const { return m_lodBias; }
  bool GetBiasClamp() const { return m_biasClamp; }
  bool GetEdgeLod() const { return m_edgeLod; }

  ChirpPaletteData& GetPaletteData() { return m_palette; }
  ChirpImageData& GetImageData() { return m_data; }

  void SetWrapS(gx::TexWrapMode wrap) { m_wrapS = wrap; }
  void SetWrapT(gx::TexWrapMode wrap) { m_wrapT = wrap; }
  void SetMinFilter(gx::TexFilter filter) { m_minFilter = filter; }
  void SetMagFilter(gx::TexFilter filter) { m_magFilter = filter; }
  void SetAnisotropy(gx::Anisotropy aniso) { m_anisotropy = aniso; }

  void SetMinLod(f32 lod) { m_minLod = lod; }
  void SetMaxLod(f32 lod) { m_maxLod = lod; }
  void SetLodBias(f32 bias) { m_lodBias = bias; }
  void SetBiasClamp(bool clamp) { m_biasClamp = clamp; }
  void SetEdgeLod(bool edgeLod) { m_edgeLod = edgeLod; }

private:
  std::string m_name;
  u16 m_width;
  u16 m_height;
  u16 m_mipCount;

  gx::TexFmt m_format;
  gx::TexFmt m_paletteFormat;
  gx::TexWrapMode m_wrapS;
  gx::TexWrapMode m_wrapT;
  gx::TexFilter m_minFilter;
  gx::TexFilter m_magFilter;
  gx::Anisotropy m_anisotropy;
  f32 m_minLod;
  f32 m_maxLod;
  f32 m_lodBias;
  bool m_biasClamp;
  bool m_edgeLod;

  ChirpPaletteData m_palette;
  ChirpImageData m_data;
};
} // namespace chirp::gx
