#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/GXEnum.hpp"
#include "libchirp/gx/GXStruct.hpp"
#include "libchirp/math/ChirpColor.hpp"

namespace chirp::gx {
class Material {
public:
  Material(const std::string& name, bool colorUpdate, bool alphaUpdate, bool zCompLoc, bool dither)
  : m_name(name)
  , m_colorUpdate(colorUpdate)
  , m_alphaUpdate(alphaUpdate)
  , m_zCompLoc(zCompLoc)
  , m_dither(dither) {
    m_stages.reserve(16);
  }
  Material(const std::string& name) : Material(name, false, false, false, false) {}

  u16 GetStageCount() const { return m_stages.size(); }

  std::vector< std::unique_ptr< TevStage > >& GetStages() { return m_stages; }
  TevStage* CreateStage() {
    if (GetStageCount() >= 16) {
      std::cerr << "Attempted to create TevStage on material \"" << m_name
                << "\", but it already had 16 stages." << std::endl;
      return nullptr;
    }

    m_stages.push_back(std::make_unique< TevStage >());
    return m_stages.back().get();
  }

  TevStage* InsertStage(u32 index) {
    if (GetStageCount() >= 16) {
      std::cerr << "Attempted to insert TevStage into material \"" << m_name
                << "\", but it already had 16 stages." << std::endl;
      return nullptr;
    }
    if (index > GetStageCount()) {
      std::cerr << "Attempted to insert TevStage into material \"" << m_name
                << "\", but the desired index is out of range." << std::endl;
      return nullptr;
    }

    m_stages.insert(m_stages.begin() + index, std::make_unique< TevStage >());
    return (m_stages.begin() + index)->get();
  }

  void RemoveStage(u32 index) {
    if (index >= GetStageCount()) {
      std::cerr << "Attempted to remove TevStage from material \"" << m_name
                << "\", but it didn't have a material at the desired index." << std::endl;
      return;
    }

    m_stages.erase(m_stages.begin() + index);
  }

  u16 GetIndirectStageCount() const { return m_indirectStages.size(); }
  std::vector< std::unique_ptr< TevIndirectStage > >& GetIndirectStages() {
    return m_indirectStages;
  }

  AlphaCompare& GetAlphaCompare() { return m_alphaCompare; }
  BlendMode& GetBlendMode() { return m_blendMode; }
  Fog& GetFog() { return m_fog; }
  ZMode& GetZMode() { return m_zMode; }

  math::ChirpColor GetTevColor(u32 idx) const {
    return idx < 16 ? m_tevColors[idx] : math::ChirpColor();
  }

  math::ChirpColor GetKonstColor(u32 idx) const {
    return idx < 16 ? m_konstColors[idx] : math::ChirpColor();
  }

  void SetTevColor(u32 idx, const math::ChirpColor& color) {
    if (idx >= 16) {
      return;
    }

    m_tevColors[idx] = color;
  }

  void SetKonstColor(u32 idx, const math::ChirpColor& color) {
    if (idx >= 16) {
      return;
    }

    m_konstColors[idx] = color;
  }

  TevKColorSel GetKonstColorSel(u32 idx) const {
    return idx < 16 ? m_konstColorSels[idx] : TevKColorSel::KColorSel_8_8;
  }

  TevKAlphaSel GetKonstAlphaSel(u32 idx) const {
    return idx < 16 ? m_konstAlphaSels[idx] : TevKAlphaSel::KAlphaSel_1;
  }

  void SetKonstColorSel(u32 idx, TevKColorSel sel) {
    if (idx >= 16) {
      return;
    }

    m_konstColorSels[idx] = sel;
  }

  void SetKonstAlphaSel(u32 idx, TevKAlphaSel sel) {
    if (idx >= 16) {
      return;
    }

    m_konstAlphaSels[idx] = sel;
  }

private:
  std::string m_name;

  bool m_colorUpdate;
  bool m_alphaUpdate;
  bool m_zCompLoc;
  bool m_dither;

  std::vector< std::unique_ptr< TevStage > > m_stages;
  std::vector< std::unique_ptr< TevIndirectStage > > m_indirectStages;

  std::array< math::ChirpColor, 16 > m_tevColors;
  std::array< math::ChirpColor, 16 > m_konstColors;
  std::array< TevKColorSel, 16 > m_konstColorSels;
  std::array< TevKAlphaSel, 16 > m_konstAlphaSels;
  std::array< TevSwapMode, 16 > m_swapModes;

  AlphaCompare m_alphaCompare;
  BlendMode m_blendMode;
  Fog m_fog;
  ZMode m_zMode;
};
} // namespace chirp::gx
