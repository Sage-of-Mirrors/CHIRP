#include "libchirp/gx/material/component/TextureComponent.hpp"

namespace chirp::gx::material {
TextureComponent::TextureComponent() {
  m_texCoordGens.reserve(8);
  m_texCoordMatrices.reserve(10);
}

std::string TextureComponent::GetTexture(u32 index) {
  return index < m_textures.size() ? m_textures[index] : "";
}

TexCoordGen* TextureComponent::GetTexCoordGen(u32 index) {
  return index < m_texCoordGens.size() ? &m_texCoordGens[index] : nullptr;
}

TexCoordMatrix* TextureComponent::GetTexCoordMatrix(u32 index) {
  return index < m_texCoordGens.size() ? &m_texCoordMatrices[index] : nullptr;
}

u32 TextureComponent::AddTexture(const std::string& name) {
  auto itr = std::find(m_textures.begin(), m_textures.end(), name);
  if (itr != m_textures.end()) {
    return itr - m_textures.begin();
  }

  u32 newIdx = static_cast< u32 >(m_textures.size());
  m_textures.push_back(name);

  return newIdx;
}

bool TextureComponent::RemoveTexture(u32 index) {
  if (index >= static_cast< u32 >(m_textures.size())) {
    return false;
  }

  m_textures.erase(m_textures.begin() + index);
  return false;
}

TexCoordGen* TextureComponent::AddTexCoordGen() {
  if (m_texCoordGens.size() >= kMaxTexGenCount) {
    return nullptr;
  }

  m_texCoordGens.emplace_back();
  return &m_texCoordGens.back();
}

bool TextureComponent::RemoveTexCoordGen(u32 index) {
  if (m_texCoordGens.empty() || index >= m_texCoordGens.size()) {
    return false;
  }

  m_texCoordGens.erase(m_texCoordGens.begin() + index);
  return true;
}

TexCoordMatrix* TextureComponent::AddTexCoordMatrix() {
  if (m_texCoordMatrices.size() >= kMaxTexMatricesCount) {
    return nullptr;
  }

  m_texCoordMatrices.emplace_back();
  return &m_texCoordMatrices.back();
}

bool TextureComponent::RemoveTexCoordMatrix(u32 index) {
  if (m_texCoordMatrices.empty() || index >= m_texCoordMatrices.size()) {
    return false;
  }

  m_texCoordMatrices.erase(m_texCoordMatrices.begin() + index);
  return true;
}
} // namespace chirp::gx::material