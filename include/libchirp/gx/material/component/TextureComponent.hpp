#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/GXEnum.hpp"

#include <array>
#include <vector>

namespace chirp::gx::material {
struct TexCoordGen {
  // How the texture coordinate is calculated.
  TexGenType m_type{TexGenType::Mtx_3_4};
  // The source vertex attribute for the texture coordinate calculation.
  TexGenSrc m_src{TexGenSrc::Position};
  // Which matrix to multiply the source attribute by.
  TexMtx m_mtx{TexMtx::TexMtx_Identity};
};

struct TexCoordMatrix {
  // Whether the matrix is 3x4 elements or 2x4 elements.
  TexMtxType m_type{TexMtxType::Mtx_3_4};
};

class TextureComponent {
public:
  static constexpr u32 kMaxTexGenCount{8};
  static constexpr u32 kMaxTexMatricesCount{10};

  TextureComponent();

  size_t GetTextureCount() const { return m_textures.size(); }
  size_t GetTexCoordGenCount() const { return m_texCoordGens.size(); }
  size_t GetTexCoordMatrixCount() const { return m_texCoordMatrices.size(); }

  std::string GetTexture(u32 index);
  TexCoordGen* GetTexCoordGen(u32 index);
  TexCoordMatrix* GetTexCoordMatrix(u32 index);

  u32 AddTexture(const std::string& name);
  bool RemoveTexture(u32 index);

  TexCoordGen* AddTexCoordGen();
  bool RemoveTexCoordGen(u32 index);

  TexCoordMatrix* AddTexCoordMatrix();
  bool RemoveTexCoordMatrix(u32 index);

private:
  std::vector< std::string > m_textures;

  std::vector< TexCoordGen > m_texCoordGens;
  std::vector< TexCoordMatrix > m_texCoordMatrices;
};
} // namespace chirp::gx::material
