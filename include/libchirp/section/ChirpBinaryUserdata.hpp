#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/ChirpKeyValue.hpp"
#include "libchirp/section/ChirpBinarySection.hpp"

#include <optional>

namespace chirp::binary {
class ChirpBinaryUserdata : public ChirpBinarySection {
public:
  ChirpBinaryUserdata();
  ChirpBinaryUserdata(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);
  ChirpBinaryUserdata(u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);

  bool Import(SectionReader& inStream) override;
  bool Export(SectionWriter& outStream) override;

  ChirpKeyValue& GetUserDataRoot() { return m_userDataRoot; }

  static std::unique_ptr< ChirpBinarySection > CreateUSRSection(u32 size, u32 elementCount,
                                                                u16 versionMajor, u16 versionMinor);
  static std::unique_ptr< ChirpBinarySection > CreateUSRSectionWithOffset(u64 offset, u32 size,
                                                                          u32 elementCount,
                                                                          u16 versionMajor,
                                                                          u16 versionMinor);

protected:
  s64 SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset = 0) override;

private:
  static constexpr uint32_t kBoolsOffsetPos{0x14};
  static constexpr uint32_t kS64sOffsetPos{0x18};
  static constexpr uint32_t kDoublesOffsetPos{0x1C};
  static constexpr uint32_t kStringsOffsetPos{0x20};

  enum class ChunkId : u8 { KeyValues, Bools, S64s, Doubles, Strings };

  void ReadOffsets(SectionReader& sectionReader);
  void ReadKeyValue(SectionReader& sectionReader, SharedChirpKeyValueTuple parentTuple);

  u32 m_keyValuesOffset{0};
  u32 m_boolsOffset{0};
  u32 m_s64sOffset{0};
  u32 m_doublesOffset{0};
  u32 m_stringsOffset{0};

  ChirpKeyValue m_userDataRoot;
};
} // namespace chirp::binary
