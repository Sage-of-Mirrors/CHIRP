#include "libchirp/section/ChirpBinaryGeo.hpp"

#include <athena/VectorWriter.hpp>

#include <cmath>

namespace chirp::binary {
ChirpMesh::ChirpMesh(const std::string& name, const std::string& materialName)
: m_name(name), m_materialName(materialName) {}
ChirpMesh::ChirpMesh(const std::string& name, const std::string& materialName,
                     const math::ChirpVector3& minBounds, const math::ChirpVector3& maxBounds)
: m_name(name), m_materialName(materialName), m_aabb(minBounds, maxBounds) {}

void ChirpMesh::ReadPrimitive(athena::io::IStreamReader& inStream) {
  ChirpBinaryPrimitive& newPrim = CreatePrimitive();

  u16 vtxCount = inStream.readUint16();
  u16 triCount = vtxCount / 3;

  for (u32 v = 0; v < triCount; v++) {
    newPrim.emplace_back();
    ChirpBinaryTriangle& newTri = newPrim.back();

    for (u8 t = 0; t < 3; t++) {
      for (u8 a = 0; a < static_cast< u8 >(gx::VtxAttribute::VtxAttributeMax); a++) {
        gx::VtxAttribute curAttr = static_cast< gx::VtxAttribute >(a);
        if (!IsVertexAttributeEnabled(curAttr)) {
          continue;
        }

        newTri[t][curAttr] = inStream.readUint16();
      }
    }
  }
}

ChirpBinaryGeo::ChirpBinaryGeo() : ChirpBinarySection() {}

ChirpBinaryGeo::ChirpBinaryGeo(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor)
: ChirpBinarySection(size, elementCount, versionMajor, versionMinor) {}

ChirpBinaryGeo::ChirpBinaryGeo(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                               u16 versionMinor)
: ChirpBinarySection(offset, size, elementCount, versionMajor, versionMinor) {}

bool ChirpBinaryGeo::Import(SectionReader& inStream) {
  m_attributesOffset = inStream.readUint32();
  m_primitivesOffset = inStream.readUint32();
  m_meshNamesOffset = inStream.readUint32();
  m_materialNamesOffset = inStream.readUint32();

  for (u32 i = 0; i < m_elementCount; i++) {
    u16 meshNameLength = inStream.readUint16();
    u16 materialNameLength = inStream.readUint16();
    u16 attributeCount = inStream.readUint16();
    u16 primitiveCount = inStream.readUint16();

    u32 meshNameOffset = inStream.readUint32();
    u32 materialNameOffset = inStream.readUint32();
    u32 vtxAttributesOffset = inStream.readUint32();
    u32 primitiveOffset = inStream.readUint32();

    math::ChirpVector3 minBounds{inStream.readFloat(), inStream.readFloat(), inStream.readFloat()};
    math::ChirpVector3 maxBounds{inStream.readFloat(), inStream.readFloat(), inStream.readFloat()};

    // Read the mesh's name.
    s64 nextMeshPos = SeekToChunk(inStream, static_cast< u8 >(ChunkId::MeshNames), meshNameOffset);
    std::string meshName = inStream.readString(meshNameLength);

    // Read the name of the mesh's material.
    SeekToChunk(inStream, static_cast< u8 >(ChunkId::MaterialNames), materialNameOffset);
    std::string materialName = inStream.readString(materialNameLength);

    ChirpMesh* newMesh = CreateMesh(meshName, materialName, minBounds, maxBounds);

    // Read the mesh's enabled vertex attributes.
    SeekToChunk(inStream, static_cast< u8 >(ChunkId::Attributes), vtxAttributesOffset);
    for (u32 a = 0; a < attributeCount; a++) {
      gx::VtxAttribute attr = static_cast< gx::VtxAttribute >(inStream.readByte());
      gx::VtxAttributeType type = static_cast< gx::VtxAttributeType >(inStream.readByte());
      newMesh->EnableVertexAttribute(attr, type);
    }

    // Read the mesh's primitives.
    SeekToChunk(inStream, static_cast< u8 >(ChunkId::Primitives), primitiveOffset);
    for (u32 p = 0; p < primitiveCount; p++) {
      newMesh->ReadPrimitive(inStream);
    }

    inStream.seek(nextMeshPos, athena::SeekOrigin::Begin);
  }

  return true;
}

bool ChirpBinaryGeo::Export(SectionWriter& outStream) {
  // Write section header.
  outStream.writeUint32(FourCC("CGEO").toUint32());
  outStream.writeUint16(m_versionMajor);
  outStream.writeUint16(m_versionMinor);
  outStream.writeUint32(0); // Placeholder for section size.
  outStream.writeUint32(static_cast< u32 >(m_meshes.size()));
  outStream.writeUint32(0); // Placeholder for attributes offset.
  outStream.writeUint32(0); // Placeholder for primitives offset.
  outStream.writeUint32(0); // Placeholder for mesh names offset.
  outStream.writeUint32(0); // Placeholder for material names offset.
  outStream.writeAlign16();

  athena::io::VectorWriter attributesWriter;
  attributesWriter.setEndian(athena::Endian::Little);
  athena::io::VectorWriter primitivesWriter;
  primitivesWriter.setEndian(athena::Endian::Little);
  athena::io::VectorWriter meshNamesWriter;
  meshNamesWriter.setEndian(athena::Endian::Little);
  athena::io::VectorWriter materialNamesWriter;
  materialNamesWriter.setEndian(athena::Endian::Little);

  for (const auto& m : m_meshes) {
    std::string meshName = m->GetName();
    std::string materialName = m->GetMaterialName();
    auto& attributes = m->GetEnabledAttributes();
    auto& primitives = m->GetPrimitives();

    // Write mesh entry.
    outStream.writeUint16(static_cast< u16 >(meshName.size() + 1)); // Add 1 for null terminator.
    outStream.writeUint16(static_cast< u16 >(materialName.size() + 1)); // ^
    outStream.writeUint16(static_cast< u16 >(attributes.size()));
    outStream.writeUint16(static_cast< u16 >(primitives.size()));

    outStream.writeUint32(static_cast< u32 >(meshNamesWriter.position()));
    meshNamesWriter.writeString(meshName);

    outStream.writeUint32(static_cast< u32 >(materialNamesWriter.position()));
    materialNamesWriter.writeString(materialName);

    outStream.writeUint32(static_cast< u32 >(attributesWriter.position()));
    for (const auto& attr : attributes) {
      attributesWriter.writeUByte(static_cast< u8 >(attr.first));
      attributesWriter.writeUByte(static_cast< u8 >(attr.second));
    }

    outStream.writeUint32(static_cast< u32 >(primitivesWriter.position()));
    for (const auto& prim : primitives) {
      primitivesWriter.writeUint16(static_cast< u16 >(
          prim.size() * 3)); // Multiply by 3 because each element here is a triangle.

      for (auto& tri : prim) {
        for (auto& vert : tri) {
          for (auto& attr : vert) {
            primitivesWriter.writeUint16(attr.second);
          }
        }
      }
    }

    outStream.writeAabb(m->GetAabb());
  }

  // Write attributes table.
  outStream.writeAlign16();
  outStream.writeOffset(kAttributesOffsetPos);
  outStream.writeUBytes(attributesWriter.data().data(), attributesWriter.length());

  // Write primitives table.
  outStream.writeAlign16();
  outStream.writeOffset(kPrimitivesOffsetPos);
  outStream.writeUBytes(primitivesWriter.data().data(), primitivesWriter.length());

  // Write mesh names table.
  outStream.writeAlign16();
  outStream.writeOffset(kMeshNamesOffsetPos);
  outStream.writeUBytes(meshNamesWriter.data().data(), meshNamesWriter.length());

  // Write material names table.
  outStream.writeAlign16();
  outStream.writeOffset(kMaterialNamesOffsetPos);
  outStream.writeUBytes(materialNamesWriter.data().data(), materialNamesWriter.length());

  outStream.writeAlign16();
  outStream.writeSectionSize();

  return true;
}

s64 ChirpBinaryGeo::SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset) {
  ChunkId chunk = static_cast< ChunkId >(chunkId);

  u32 chunkOffset{0};
  switch (chunk) {
  case ChunkId::Attributes:
    chunkOffset = m_attributesOffset;
    break;
  case ChunkId::Primitives:
    chunkOffset = m_primitivesOffset;
    break;
  case ChunkId::MeshNames:
    chunkOffset = m_meshNamesOffset;
    break;
  case ChunkId::MaterialNames:
    chunkOffset = m_materialNamesOffset;
    break;
  default:
    break;
  }

  s64 curPos = sectionReader.position();
  sectionReader.seek(m_offset + chunkOffset + dataOffset, athena::SeekOrigin::Begin);

  return curPos;
}

ChirpMesh* ChirpBinaryGeo::CreateMesh(const std::string& name, const std::string& materialName,
                                      const math::ChirpVector3& minBounds,
                                      const math::ChirpVector3& maxBounds) {
  auto newMesh = std::make_unique< ChirpMesh >(name, materialName, minBounds, maxBounds);
  m_meshes.push_back(std::move(newMesh));

  return m_meshes.back().get();
}

std::unique_ptr< ChirpBinarySection > ChirpBinaryGeo::CreateGEOSection(const u32 size,
                                                                       const u32 elementCount,
                                                                       u16 versionMajor,
                                                                       u16 versionMinor) {
  return std::make_unique< ChirpBinaryGeo >(size, elementCount, versionMajor, versionMinor);
}
std::unique_ptr< ChirpBinarySection >
ChirpBinaryGeo::CreateGEOSectionWithOffset(const u64 offset, const u32 size, const u32 elementCount,
                                           u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinaryGeo >(offset, size, elementCount, versionMajor, versionMinor);
}
} // namespace chirp::binary
