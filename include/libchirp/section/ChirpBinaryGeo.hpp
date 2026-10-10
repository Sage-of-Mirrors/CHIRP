#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/gx/GXEnum.hpp"
#include "libchirp/section/ChirpBinarySection.hpp"

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
    return true;
  }

  gx::VtxAttributeType GetVertexAttributeType(gx::VtxAttribute attr) const {
    if (auto itr = m_enabledAttributes.find(attr); itr != m_enabledAttributes.end()) {
      return itr->second;
    }

    return gx::VtxAttributeType::None;
  }

  ChirpBinaryPrimitive& CreatePrimitive() {
    m_primitives.emplace_back();
    return m_primitives.back();
  }

  void ReadPrimitive(athena::io::IStreamReader& inStream);

private:
  std::string m_name;
  std::string m_materialName;
  std::unordered_map< gx::VtxAttribute, gx::VtxAttributeType > m_enabledAttributes;
  std::vector< ChirpBinaryPrimitive > m_primitives;
};

class ChirpBinaryGeo : public ChirpBinarySection {
public:
  ChirpBinaryGeo();
  ChirpBinaryGeo(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);
  ChirpBinaryGeo(u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);

  bool Import(SectionReader& inStream) override;
  bool Export(athena::io::IStreamWriter& outStream) override;

  ChirpMesh* CreateMesh(std::string_view name, std::string_view materialName);

  std::vector< std::unique_ptr< ChirpMesh > >& GetMeshes() { return m_meshes; }

  static std::unique_ptr< ChirpBinarySection > CreateGEOSection(u32 size, u32 elementCount,
                                                                u16 versionMajor, u16 versionMinor);
  static std::unique_ptr< ChirpBinarySection > CreateGEOSectionWithOffset(u64 offset, u32 size,
                                                                          u32 elementCount,
                                                                          u16 versionMajor,
                                                                          u16 versionMinor);

protected:
  s64 SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset = 0) override;

private:
  enum class ChunkId : u8 { Attributes, Primitives, MeshNames, MaterialNames };

  u32 m_attributesOffset{0};
  u32 m_primitivesOffset{0};
  u32 m_meshNamesOffset{0};
  u32 m_materialNamesOffset{0};

  std::vector< std::unique_ptr< ChirpMesh > > m_meshes;
};
} // namespace chirp::binary
