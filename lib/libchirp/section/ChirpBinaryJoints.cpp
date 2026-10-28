#include "libchirp/section/ChirpBinaryJoints.hpp"

#include <athena/VectorWriter.hpp>

namespace chirp::binary {
ChirpBinaryJoints::ChirpBinaryJoints() : ChirpBinarySection() {}
ChirpBinaryJoints::ChirpBinaryJoints(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor)
: ChirpBinarySection(size, elementCount, versionMajor, versionMinor) {}
ChirpBinaryJoints::ChirpBinaryJoints(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                     u16 versionMinor)
: ChirpBinarySection(offset, size, elementCount, versionMajor, versionMinor) {}

bool ChirpBinaryJoints::Import(SectionReader& inStream) {
  m_childIndicesOffset = inStream.readUint32();
  m_jointNamesOffset = inStream.readUint32();
  inStream.seekAlign16();

  for (u32 i = 0; i < m_elementCount; i++) {
    u16 nameLength = inStream.readUint16();
    u16 parentIndex = inStream.readUint16();
    u16 childCount = inStream.readUint16();
    u16 flags = inStream.readUint16();

    u32 nameOffset = inStream.readUint32();
    u32 firstChildIndexOffset = inStream.readUint32();

    s64 returnPos = SeekToChunk(inStream, static_cast< u8 >(ChunkId::JointNames), nameOffset);
    std::string name = inStream.readString(nameLength);

    ChirpJoint* newJoint = CreateJoint(name, parentIndex);

    if (childCount) {
      SeekToChunk(inStream, static_cast< u8 >(ChunkId::ChildIndices), firstChildIndexOffset);
      for (u32 c = 0; c < childCount; c++) {
        newJoint->AddChildIndex(inStream.readUint16());
      }
    }

    inStream.seek(returnPos, athena::SeekOrigin::Begin);

    chirp::math::ChirpTransform& jointTransform = newJoint->GetTransform();
    jointTransform.Position().X() = inStream.readFloat();
    jointTransform.Position().Y() = inStream.readFloat();
    jointTransform.Position().Z() = inStream.readFloat();

    jointTransform.Rotation().X() = inStream.readFloat();
    jointTransform.Rotation().Y() = inStream.readFloat();
    jointTransform.Rotation().Z() = inStream.readFloat();
    jointTransform.Rotation().W() = inStream.readFloat();

    jointTransform.Scale().X() = inStream.readFloat();
    jointTransform.Scale().Y() = inStream.readFloat();
    jointTransform.Scale().Z() = inStream.readFloat();
  }

  return true;
}
bool ChirpBinaryJoints::Export(SectionWriter& outStream) {
  // Write section header.
  outStream.writeUint32(FourCC("CJNT").toUint32());
  outStream.writeUint16(m_versionMajor);
  outStream.writeUint16(m_versionMinor);
  outStream.writeUint32(0); // Placeholder for section size.
  outStream.writeUint32(static_cast< u32 >(m_joints.size()));
  outStream.writeUint32(0); // Placeholder for child indices offset.
  outStream.writeUint32(0); // Placeholder for names offset.
  outStream.writeAlign16();

  athena::io::VectorWriter childIndicesWriter;
  childIndicesWriter.setEndian(athena::Endian::Little);
  athena::io::VectorWriter jointNamesWriter;
  jointNamesWriter.setEndian(athena::Endian::Little);

  for (const auto& j : m_joints) {
    std::string name = j->GetName();
    auto& childIndices = j->GetChildIndices();

    // Write joint entry.
    outStream.writeUint16(static_cast< u16 >(name.size() + 1)); // Add 1 for the null-terminator.
    outStream.writeUint16(j->GetParentIndex());
    outStream.writeUint16(static_cast< u16 >(childIndices.size()));
    outStream.writeUint16(0);

    outStream.writeUint32(static_cast< u32 >(jointNamesWriter.position()));
    jointNamesWriter.writeString(name);

    outStream.writeUint32(static_cast< u32 >(childIndicesWriter.position()));
    for (const u16& c : childIndices) {
      childIndicesWriter.writeUint16(c);
    }

    outStream.writeTransform(j->GetTransform());
  }

  // Write accumulated child indices, if there are any.
  if (childIndicesWriter.length()) {
    outStream.writeAlign16();
    outStream.writeOffset(kChildIndicesOffsetPos);
    outStream.writeUBytes(childIndicesWriter.data().data(), childIndicesWriter.length());
  }

  // Write accumulated joint names.
  outStream.writeAlign16();
  outStream.writeOffset(kJointNamesOffsetPos);
  outStream.writeUBytes(jointNamesWriter.data().data(), jointNamesWriter.length());

  outStream.writeAlign16();
  outStream.writeSectionSize();

  return true;
}

s64 ChirpBinaryJoints::SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset) {
  ChunkId chunk = static_cast< ChunkId >(chunkId);

  u32 chunkOffset{0};
  switch (chunk) {
  case ChunkId::ChildIndices:
    chunkOffset = m_childIndicesOffset;
    break;
  case ChunkId::JointNames:
    chunkOffset = m_jointNamesOffset;
    break;
  default:
    break;
  }

  s64 curPos = sectionReader.position();
  sectionReader.seek(m_offset + chunkOffset + dataOffset, athena::SeekOrigin::Begin);

  return curPos;
}

ChirpJoint* ChirpBinaryJoints::CreateJoint(const std::string& name, u16 parentIndex) {
  std::string nameAsStr(name);
  if (m_jointNameMap.contains(nameAsStr)) {
    return m_jointNameMap[nameAsStr];
  }

  std::unique_ptr< ChirpJoint > newJoint = std::make_unique< ChirpJoint >(name, parentIndex);
  m_joints.push_back(std::move(newJoint));
  m_jointNameMap[nameAsStr] = m_joints.back().get();

  return m_joints.back().get();
}

std::unique_ptr< ChirpBinarySection > ChirpBinaryJoints::CreateJNTSection(u32 size,
                                                                          u32 elementCount,
                                                                          u16 versionMajor,
                                                                          u16 versionMinor) {
  return std::make_unique< ChirpBinaryJoints >(size, elementCount, versionMajor, versionMinor);
}
std::unique_ptr< ChirpBinarySection >
ChirpBinaryJoints::CreateJNTSectionWithOffset(u64 offset, u32 size, u32 elementCount,
                                              u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinaryJoints >(offset, size, elementCount, versionMajor,
                                               versionMinor);
}
} // namespace chirp::binary
