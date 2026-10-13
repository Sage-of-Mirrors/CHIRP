#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/GXEnum.hpp"

#include <vector>

namespace chirp::gx {
class ChirpTexture {
public:
  using ChirpImageData = std::vector< u8 >;

  ChirpTexture(std::string_view name, u16 width, u16 height, gx::TexFmt format, u16 mipCount,
               gx::TexWrapMode wrapS, gx::TexWrapMode wrapT, gx::TexFilter minFilter,
               gx::TexFilter magFilter, f32 minLod, f32 maxLod, f32 lodBias)
  : m_name(name)
  , m_width(width)
  , m_height(height)
  , m_format(format)
  , m_mipCount(mipCount)
  , m_wrapS(wrapS)
  , m_wrapT(wrapT)
  , m_minFilter(minFilter)
  , m_magFilter(magFilter)
  , m_minLod(minLod)
  , m_maxLod(maxLod)
  , m_lodBias(lodBias) {}

  ChirpTexture(std::string_view name, u16 width, u16 height, gx::TexFmt format)
  : ChirpTexture(name, width, height, format, 1, gx::TexWrapMode::Clamp, gx::TexWrapMode::Clamp,
                 gx::TexFilter::Linear, gx::TexFilter::Linear, 0.0f, 1.0f, 0.0f) {}

  std::string GetName() const { return m_name; }
  u16 GetWidth() const { return m_width; }
  u16 GetHeight() const { return m_height; }
  u16 GetMipCount() const { return m_mipCount; }
  gx::TexFmt GetFormat() const { return m_format; }
  gx::TexWrapMode GetWrapS() const { return m_wrapS; }
  gx::TexWrapMode GetWrapT() const { return m_wrapT; }
  gx::TexFilter GetMinFilter() const { return m_minFilter; }
  gx::TexFilter GetMagFilter() const { return m_magFilter; }
  f32 GetMinLod() const { return m_minLod; }
  f32 GetMaxLod() const { return m_maxLod; }
  f32 GetLodBias() const { return m_lodBias; }

  ChirpImageData& GetImageData() { return m_data; }

  void SetWrapS(gx::TexWrapMode wrap) { m_wrapS = wrap; }
  void SetWrapT(gx::TexWrapMode wrap) { m_wrapT = wrap; }
  void SetMinFilter(gx::TexFilter filter) { m_minFilter = filter; }
  void SetMagFilter(gx::TexFilter filter) { m_magFilter = filter; }
  void SetMinLod(f32 lod) { m_minLod = lod; }
  void SetMaxLod(f32 lod) { m_maxLod = lod; }
  void SetLodBias(f32 bias) { m_lodBias = bias; }

private:
  std::string m_name;
  u16 m_width;
  u16 m_height;
  u16 m_mipCount;

  gx::TexFmt m_format;
  gx::TexWrapMode m_wrapS;
  gx::TexWrapMode m_wrapT;
  gx::TexFilter m_minFilter;
  gx::TexFilter m_magFilter;
  f32 m_minLod;
  f32 m_maxLod;
  f32 m_lodBias;

  ChirpImageData m_data;
};
} // namespace chirp::gx
