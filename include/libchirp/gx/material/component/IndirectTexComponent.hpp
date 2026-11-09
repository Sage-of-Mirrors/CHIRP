#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/GXEnum.hpp"
#include "libchirp/math/ChirpMatrix2x3.hpp"

namespace chirp::gx::material {
struct TevIndirectTexOrder {
  IndirectTexStageId m_indirectStageId;
  TexCoordId m_texCoordId;
  TexMapId m_texMapId;
};

struct TevIndirectTile {
  u8 m_affectedTevStage;
  IndirectTexStageId m_indirectStageId;
  u16 m_tileSizeS;
  u16 m_tileSizeT;
  u16 m_tileSpacingS;
  u16 m_tileSpacingT;
  IndirectTexFormat m_texFormat;
  IndirectTexMtxId m_texMtxId;
  IndirectTexBiasSel m_texBiasSel;
  IndirectTexAlphaSel m_alphaSel;
};

struct TevIndirectStage {
  u8 m_affectedTevStage;
  IndirectTexStageId m_texStageId;
  IndirectTexFormat m_texFmt;
  IndirectTexBiasSel m_texBiasSel;
  IndirectTexMtxId m_texMtxId;
  IndirectTexWrap m_texWrapS;
  IndirectTexWrap m_texWrapT;
  bool m_addPrev;
  bool m_indirectLod;
  IndirectTexAlphaSel m_alphaSel;
};

struct TevIndirectTexCoordScale {
  IndirectTexScale m_scaleS;
  IndirectTexScale m_scaleT;
};

struct TevIndirectTexMatrix {
  IndirectTexMtxId m_texMtxId;
  s8 m_scaleExponent;

  math::ChirpMatrix2x3 m_texMatrix;
};

class IndirectTexComponent {
public:
private:
};
} // namespace chirp::gx::material
