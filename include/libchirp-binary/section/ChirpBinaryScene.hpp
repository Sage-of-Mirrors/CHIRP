#pragma once

#include <libchirp-common/ChirpCommon.hpp>

#include "libchirp-binary/section/ChirpBinarySection.hpp"

#include <libchirp-common/math/ChirpTransform.hpp>

#include <map>

namespace chirp::binary {
class ChirpJoint {
public:
  ChirpJoint(std::string_view name, u16 parentIndex) : m_name(name), m_parentIndex(parentIndex) {}

  std::string GetName() const { return m_name; }
  u16 GetParentIndex() const { return m_parentIndex; }
  std::vector< u16 >& GetChildIndices() { return m_childIndices; }
  chirp::math::ChirpTransform& GetTransform() { return m_transform; }

  void AddChildIndex(u16 index) { m_childIndices.push_back(index); }

private:
  std::string m_name;

  u16 m_parentIndex;
  std::vector< u16 > m_childIndices;

  chirp::math::ChirpTransform m_transform;
};

class ChirpBinaryScene : public ChirpBinarySection {
public:
  ChirpBinaryScene();
  ChirpBinaryScene(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);
  ChirpBinaryScene(u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);

  bool Import(SectionReader& inStream) override;
  bool Export(athena::io::IStreamWriter& outStream) override;

  ChirpJoint* CreateJoint(std::string_view name,
                          u16 parentIndex = std::numeric_limits< u16 >::max());

  std::vector< std::unique_ptr< ChirpJoint > >& GetJoints() { return m_joints; }
  ChirpJoint* GetJoint(std::string_view name) {
    std::string nameAsStr(name);
    return m_jointNameMap.contains(nameAsStr) ? m_jointNameMap[nameAsStr] : nullptr;
  }
  ChirpJoint* GetJoint(u16 index) {
    return index < m_joints.size() ? m_joints[index].get() : nullptr;
  }

  static std::unique_ptr< ChirpBinarySection > CreateSCNSection(u32 size, u32 elementCount,
                                                                u16 versionMajor, u16 versionMinor);
  static std::unique_ptr< ChirpBinarySection > CreateSCNSectionWithOffset(u64 offset, u32 size,
                                                                          u32 elementCount,
                                                                          u16 versionMajor,
                                                                          u16 versionMinor);

protected:
  s64 SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset = 0) override;

private:
  enum class ChunkId : u8 { ChildIndices, JointNames };

  u32 m_childIndicesOffset{0};
  u32 m_jointNamesOffset{0};

  std::vector< std::unique_ptr< ChirpJoint > > m_joints;
  std::map< std::string, ChirpJoint* > m_jointNameMap;
};
} // namespace chirp::binary
