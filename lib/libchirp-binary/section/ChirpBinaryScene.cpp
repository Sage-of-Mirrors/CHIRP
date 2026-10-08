#include "libchirp-binary/section/ChirpBinaryScene.hpp"

#include <athena/IStreamReader.hpp>
#include <athena/IStreamWriter.hpp>

namespace chirp::binary {
ChirpBinaryScene::ChirpBinaryScene() : ChirpBinarySection() {}
ChirpBinaryScene::ChirpBinaryScene(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Scenegraph, size, elementCount, versionMajor, versionMinor) {
}
ChirpBinaryScene::ChirpBinaryScene(u64 offset, u32 size, u32 elementCount, u16 versionMajor,
                                   u16 versionMinor)
: ChirpBinarySection(ChirpSectionType::Scenegraph, offset, size, elementCount, versionMajor,
                     versionMinor) {}

bool ChirpBinaryScene::Import(athena::io::IStreamReader& inStream) {
  u32 childIndexTableOffset = inStream.readUint32();
  u32 nameTableOffset = inStream.readUint32();
  inStream.seekAlign16();

  for (u32 i = 0; i < m_elementCount; i++) {
    u16 nameLength = inStream.readUint16();
    u16 parentIndex = inStream.readUint16();
    u16 childCount = inStream.readUint16();
    u16 flags = inStream.readUint16();

    u32 nameOffset = inStream.readUint32();
    u32 firstChildIndexOffset = inStream.readUint32();

    u64 curPos = inStream.position();

    inStream.seek(m_offset + nameTableOffset + nameOffset, athena::SeekOrigin::Begin);
    std::string name = inStream.readString(nameLength);

    ChirpJoint* newJoint = CreateJoint(name, parentIndex);

    if (childCount) {
      inStream.seek(m_offset + childIndexTableOffset + firstChildIndexOffset,
                    athena::SeekOrigin::Begin);
      for (u32 c = 0; c < childCount; c++) {
        newJoint->AddChildIndex(inStream.readUint16());
      }
    }

    inStream.seek(curPos, athena::SeekOrigin::Begin);

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
bool ChirpBinaryScene::Export(athena::io::IStreamWriter& outStream) { return true; }

ChirpJoint* ChirpBinaryScene::CreateJoint(std::string_view name, u16 parentIndex) {
  std::string nameAsStr(name);
  if (m_jointNameMap.contains(nameAsStr)) {
    return m_jointNameMap[nameAsStr];
  }

  std::unique_ptr< ChirpJoint > newJoint = std::make_unique< ChirpJoint >(name, parentIndex);
  m_joints.push_back(std::move(newJoint));
  m_jointNameMap[nameAsStr] = m_joints.back().get();

  return m_joints.back().get();
}

std::unique_ptr< ChirpBinarySection >
ChirpBinaryScene::CreateSCNSection(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinaryScene >(
      ChirpBinaryScene(size, elementCount, versionMajor, versionMinor));
}
std::unique_ptr< ChirpBinarySection >
ChirpBinaryScene::CreateSCNSectionWithOffset(u64 offset, u32 size, u32 elementCount,
                                             u16 versionMajor, u16 versionMinor) {
  return std::make_unique< ChirpBinaryScene >(
      ChirpBinaryScene(offset, size, elementCount, versionMajor, versionMinor));
}
} // namespace chirp::binary
