#pragma once

#include <libchirp-common/ChirpCommon.hpp>

#include "libchirp-binary/section/ChirpBinarySection.hpp"

#include <libchirp-common/gx/GXEnum.hpp>
#include <libchirp-common/math/ChirpVector.hpp>

#include <unordered_map>
#include <vector>

namespace chirp::binary {
class ChirpVertexAttribute {
public:
  ChirpVertexAttribute(gx::VtxAttribute attribute, gx::VtxComponentSize compSize,
                       gx::VtxComponentType compType, u8 exponent = 0)
  : m_attribute(attribute)
  , m_componentSize(compSize)
  , m_componentType(compType)
  , m_exponent(exponent) {}

  ChirpVertexAttribute(gx::VtxAttribute attribute, const std::vector< math::ChirpVector4 >& data,
                       gx::VtxComponentSize compSize, gx::VtxComponentType compType,
                       u8 exponent = 0)
  : m_attribute(attribute)
  , m_data(data)
  , m_componentSize(compSize)
  , m_componentType(compType)
  , m_exponent(exponent) {}

  gx::VtxAttribute GetAttribute() const { return m_attribute; }
  gx::VtxComponentSize GetComponentSize() const { return m_componentSize; }
  gx::VtxComponentType GetComponentType() const { return m_componentType; }
  u8 GetExponent() const { return m_exponent; }

  std::vector< math::ChirpVector4 >& GetData() { return m_data; }

private:
  gx::VtxAttribute m_attribute{gx::VtxAttribute::PositionMatrixIndex};
  gx::VtxComponentSize m_componentSize{gx::VtxComponentSize::PositionXy};
  gx::VtxComponentType m_componentType{gx::VtxComponentType::Uint8};
  u8 m_exponent{0};

  std::vector< math::ChirpVector4 > m_data;
};

class ChirpBinaryVat : public ChirpBinarySection {
public:
  ChirpBinaryVat();
  ChirpBinaryVat(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);
  ChirpBinaryVat(u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);

  bool Import(athena::io::IStreamReader& inStream) override;
  bool Export(athena::io::IStreamWriter& outStream) override;

  ChirpVertexAttribute* GetVertexAttribute(gx::VtxAttribute attr);

  ChirpVertexAttribute* CreateVertexAttribute(gx::VtxAttribute attr, gx::VtxComponentSize size,
                                              gx::VtxComponentType type, u8 exponent = 0);
  ChirpVertexAttribute* CreateVertexAttribute(gx::VtxAttribute attr,
                                              const std::vector< math::ChirpVector4 >& data,
                                              gx::VtxComponentSize size, gx::VtxComponentType type,
                                              u8 exponent = 0);

  static std::unique_ptr< ChirpBinarySection > CreateVATSection(u32 size, u32 elementCount,
                                                                u16 versionMajor, u16 versionMinor);
  static std::unique_ptr< ChirpBinarySection > CreateVATSectionWithOffset(u64 offset, u32 size,
                                                                          u32 elementCount,
                                                                          u16 versionMajor,
                                                                          u16 versionMinor);

private:
  void ReadVertexAttributeData(athena::io::IStreamReader& inStream, u32 count,
                               ChirpVertexAttribute* attribute);
  void WriteVertexAttributeData(athena::io::IStreamWriter& outStream, u32 count,
                                ChirpVertexAttribute* attribute);

  std::unordered_map< gx::VtxAttribute, std::unique_ptr< ChirpVertexAttribute > > m_attributes;
};
} // namespace chirp::binary
