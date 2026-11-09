#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/material/component/ColorComponent.hpp"
#include "libchirp/gx/material/component/CombinerComponent.hpp"
#include "libchirp/gx/material/component/IndirectTexComponent.hpp"
#include "libchirp/gx/material/component/LightingComponent.hpp"
#include "libchirp/gx/material/component/PixelEngineComponent.hpp"
#include "libchirp/gx/material/component/TextureComponent.hpp"

namespace chirp::gx::material {
class Material {
public:
  Material(const std::string& name) : m_name(name) {}

  std::string GetName() { return m_name; }
  u32 GetTevStageCount() { return m_combinerComponent.GetTevStageCount(); }

  ColorComponent& GetColorComponent() { return m_colorComponent; }
  CombinerComponent& GetCombinerComponent() { return m_combinerComponent; }
  IndirectTexComponent& GetIndirectTexComponent() { return m_indirectTexComponent; }
  LightingComponent& GetLightingComponent() { return m_lightingComponent; }
  PixelEngineComponent& GetPixelEngineComponent() { return m_pixelEngineComponent; }
  TextureComponent& GetTextureComponent() { return m_textureComponent; }

  const ColorComponent& GetColorComponent() const { return m_colorComponent; }
  const CombinerComponent& GetCombinerComponent() const { return m_combinerComponent; }
  const IndirectTexComponent& GetIndirectTexComponent() const { return m_indirectTexComponent; }
  const LightingComponent& GetLightingComponent() const { return m_lightingComponent; }
  const PixelEngineComponent& GetPixelEngineComponent() const { return m_pixelEngineComponent; }
  const TextureComponent& GetTextureComponent() const { return m_textureComponent; }

private:
  std::string m_name;

  ColorComponent m_colorComponent{};
  CombinerComponent m_combinerComponent{};
  IndirectTexComponent m_indirectTexComponent{};
  LightingComponent m_lightingComponent{};
  PixelEngineComponent m_pixelEngineComponent{};
  TextureComponent m_textureComponent{};
};
} // namespace chirp::gx::material
