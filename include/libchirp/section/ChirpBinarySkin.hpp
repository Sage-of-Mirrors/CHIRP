#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/math/ChirpMatrix3x4.hpp"
#include "libchirp/section/ChirpBinarySection.hpp"

#include <vector>

namespace chirp::binary {
constexpr u32 kMaxSkinBindCount{10};

using ChirpSkinIndices = std::array< u16, kMaxSkinBindCount >;
using ChirpSkinWeights = std::array< f32, kMaxSkinBindCount >;

class ChirpSkinBinding {
public:
  ChirpSkinBinding() : m_bindCount(0) {}
  ChirpSkinBinding(u16 count, const ChirpSkinIndices& indices, const ChirpSkinWeights& weights)
  : m_bindCount(count), m_indices(indices), m_weights(weights) {}

  u16 GetBindingCount() const { return m_bindCount; }

  ChirpSkinIndices& GetIndices() { return m_indices; }
  ChirpSkinWeights& GetWeights() { return m_weights; }

  void AddBinding(u16 index, f32 weight) {
    if (m_bindCount >= kMaxSkinBindCount) {
      return;
    }

    m_indices[m_bindCount] = index;
    m_weights[m_bindCount] = weight;

    m_bindCount++;
  }

private:
  u16 m_bindCount{0};
  ChirpSkinIndices m_indices{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  ChirpSkinWeights m_weights{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
};

class ChirpBinarySkin : public ChirpBinarySection {
public:
  ChirpBinarySkin();
  ChirpBinarySkin(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);
  ChirpBinarySkin(u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);

  bool Import(SectionReader& inStream) override;
  bool Export(athena::io::IStreamWriter& outStream) override;

  size_t GetBindingCount() const { return m_bindings.size(); }
  size_t GetInverseBindMatrixCount() const { return m_inverseBindMatrices.size(); }

  std::vector< std::unique_ptr< ChirpSkinBinding > >& GetBindings() { return m_bindings; }
  ChirpSkinBinding* GetBinding(u32 index) {
    if (index >= GetBindingCount()) {
      return nullptr;
    }

    return m_bindings[index].get();
  }

  std::vector< math::ChirpMatrix3x4 >& GetInverseBindMatrices() { return m_inverseBindMatrices; }
  math::ChirpMatrix3x4* GetInverseBindMatrix(u32 index) {
    if (index >= GetInverseBindMatrixCount()) {
      return nullptr;
    }

    return &m_inverseBindMatrices[index];
  }

  ChirpSkinBinding* CreateSkinBinding() {
    m_bindings.push_back(std::make_unique< ChirpSkinBinding >());
    return m_bindings.back().get();
  }

  ChirpSkinBinding* CreateSkinBinding(u16 bindingCount, const ChirpSkinIndices& indices,
                                      const ChirpSkinWeights& weights) {
    m_bindings.push_back(std::make_unique< ChirpSkinBinding >(bindingCount, indices, weights));
    return m_bindings.back().get();
  }

  math::ChirpMatrix3x4& AddInverseBindMatrix();
  math::ChirpMatrix3x4& AddInverseBindMatrix(const math::ChirpVector4& r0,
                                             const math::ChirpVector4& r1,
                                             const math::ChirpVector4& r2);

  static std::unique_ptr< ChirpBinarySection > CreateSKNSection(u32 size, u32 elementCount,
                                                                u16 versionMajor, u16 versionMinor);
  static std::unique_ptr< ChirpBinarySection > CreateSKNSectionWithOffset(u64 offset, u32 size,
                                                                          u32 elementCount,
                                                                          u16 versionMajor,
                                                                          u16 versionMinor);

protected:
  s64 SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset = 0) override;

private:
  enum class ChunkId : u8 { Indices, Weights, Matrices };

  u32 m_inverseBindMatrixCount{0};
  u32 m_indicesOffset{0};
  u32 m_weightsOffset{0};
  u32 m_matricesOffset{0};

  std::vector< std::unique_ptr< ChirpSkinBinding > > m_bindings;
  std::vector< math::ChirpMatrix3x4 > m_inverseBindMatrices;
};
} // namespace chirp::binary
