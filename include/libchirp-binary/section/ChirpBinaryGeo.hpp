#pragma once

#include <libchirp-common/ChirpCommon.hpp>
#include <libchirp-common/gx/GXEnum.hpp>

#include "libchirp-binary/section/ChirpBinarySection.hpp"

#include <array>
#include <unordered_map>
#include <vector>

namespace chirp::binary {
using ChirpBinaryVertex = std::unordered_map< gx::VtxAttribute, u16 >;
using ChirpBinaryTriangle = std::array< ChirpBinaryVertex, 3 >;
using ChirpBinaryPrimitive = std::vector< ChirpBinaryTriangle >;

class ChirpMesh {
public:
  ChirpMesh(std::string_view name, std::string_view materialName);

  std::string GetName() const { return m_name; }
  std::string GetMaterialName() const { return m_materialName; }

  bool IsVertexAttributeEnabled(gx::VtxAttribute attr) const {
    return m_enabledAttributes.contains(attr);
  }

  bool EnableVertexAttribute(gx::VtxAttribute attr,
                             gx::VtxAttributeType type = gx::VtxAttributeType::Index16) {
    if (IsVertexAttributeEnabled(attr)) {
      std::cerr << "Attempted to enable vertex attribute on a ChirpMesh that already has it."
                << std::endl;
      return false;
    }

    m_enabledAttributes[attr] = type;
  }

  gx::VtxAttributeType GetVertexAttributeType(gx::VtxAttribute attr) const {
    if (auto itr = m_enabledAttributes.find(attr); itr != m_enabledAttributes.end()) {
      return itr->second;
    }

    return gx::VtxAttributeType::None;
  }

private:
  std::string m_name;
  std::string m_materialName;
  std::unordered_map< gx::VtxAttribute, gx::VtxAttributeType > m_enabledAttributes;
  std::vector< ChirpBinaryPrimitive > m_primitives;
};

class ChirpBinaryGeo : public ChirpBinarySection {
public:
  ChirpBinaryGeo();
  ChirpBinaryGeo(u32 size, u32 elementCount);
  ChirpBinaryGeo(u64 offset, u32 size, u32 elementCount);

  bool Import(athena::io::IStreamReader& inStream) override;
  bool Export(athena::io::IStreamWriter& outStream) override;

  ChirpMesh* CreateMesh(std::string_view name, std::string_view materialName);

  std::vector< std::unique_ptr< ChirpMesh > >& GetMeshes() { return m_meshes; }

  static std::unique_ptr< ChirpBinarySection > CreateGEOSection(u32 size, u32 elementCount);
  static std::unique_ptr< ChirpBinarySection > CreateGEOSectionWithOffset(u64 offset, u32 size,
                                                                          u32 elementCount);

private:
  std::vector< std::unique_ptr< ChirpMesh > > m_meshes;
};
} // namespace chirp::binary
