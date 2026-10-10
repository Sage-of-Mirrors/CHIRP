#include "libchirp/section/ChirpBinaryGeo.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>

#include <cmath>

namespace chirp::binary {
ChirpMesh::ChirpMesh(std::string_view name, std::string_view materialName)
: m_name(name), m_materialName(materialName) {}

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
: ChirpBinarySection(ChirpSectionType::Geometry, size, elementCount, versionMajor, versionMinor) {}

ChirpBinaryGeo::ChirpBinaryGeo(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                               u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Geometry, offset, size, elementCount, versionMajor,
                     versionMinor) {}

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

    // Read the mesh's name.
    s64 nextMeshPos = SeekToChunk(inStream, static_cast< u8 >(ChunkId::MeshNames), meshNameOffset);
    std::string meshName = inStream.readString(meshNameLength);

    // Read the name of the mesh's material.
    SeekToChunk(inStream, static_cast< u8 >(ChunkId::MaterialNames), materialNameOffset);
    std::string materialName = inStream.readString(materialNameLength);

    ChirpMesh* newMesh = CreateMesh(meshName, materialName);

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

bool ChirpBinaryGeo::Export(athena::io::IStreamWriter& outStream) { return true; }

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

ChirpMesh* ChirpBinaryGeo::CreateMesh(std::string_view name, std::string_view materialName) {
  auto newMesh = std::make_unique< ChirpMesh >(name, materialName);
  m_meshes.push_back(std::move(newMesh));

  return m_meshes.back().get();
}

std::unique_ptr< ChirpBinarySection > ChirpBinaryGeo::CreateGEOSection(const u32 size,
                                                                       const u32 elementCount,
                                                                       u16 versionMajor,
                                                                       u16 versionMinor) {
  return std::make_unique< ChirpBinaryGeo >(
      ChirpBinaryGeo(size, elementCount, versionMajor, versionMinor));
}
std::unique_ptr< ChirpBinarySection >
ChirpBinaryGeo::CreateGEOSectionWithOffset(const u64 offset, const u32 size, const u32 elementCount,
                                           u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinaryGeo >(
      ChirpBinaryGeo(offset, size, elementCount, versionMajor, versionMinor));
}
} // namespace chirp::binary
