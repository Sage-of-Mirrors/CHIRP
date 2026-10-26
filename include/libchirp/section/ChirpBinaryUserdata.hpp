#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/ChirpKeyValue.hpp"
#include "libchirp/section/ChirpBinarySection.hpp"

namespace chirp::binary {
class ChirpUserData;

using ChirpUserDataChildren = std::unordered_map< std::string, ChirpUserData >;

class ChirpUserData {
public:
  ChirpUserData(std::string_view name) : m_namespace(name) {}
  ChirpUserData() : ChirpUserData("") {}

  std::string GetNamespace() const { return m_namespace; }
  ChirpKeyValueTuple& GetFields() { return m_fields; }
  ChirpUserDataChildren& GetChildren() { return m_children; }

  bool HasField(std::string_view key) const { return m_fields.contains(std::string(key)); }
  bool HasChild(std::string_view key) const { return m_children.contains(std::string(key)); }

  ChirpKeyValue& GetField(std::string_view key);
  ChirpUserData& GetChild(std::string_view key);

  ChirpKeyValue* CreateField(std::string_view name);
  ChirpUserData* CreateChild(std::string_view name);

  template < typename T >
  T Get(std::string_view key) {
    if (!HasField(key)) {
      return T{};
    }

    return m_fields[std::string(key)]->Get< T >();
  }

  ChirpKeyValue& operator[](std::string_view key);

private:
  std::string m_namespace;
  ChirpKeyValueTuple m_fields;
  ChirpUserDataChildren m_children;
};

class ChirpBinaryUserdata : public ChirpBinarySection {
public:
  ChirpBinaryUserdata();
  ChirpBinaryUserdata(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);
  ChirpBinaryUserdata(u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);

  bool Import(SectionReader& inStream) override;
  bool Export(SectionWriter& outStream) override;

  ChirpUserData* GetUserData(std::string_view name) {
    std::string nameAsStr(name);

    if (!m_userData.contains(nameAsStr)) {
      return nullptr;
    }
    return m_userData[nameAsStr].get();
  }

  ChirpUserData* CreateUserData(std::string_view name) {
    std::string nameAsStr(name);

    if (m_userData.contains(nameAsStr)) {
      return nullptr;
    }

    m_userData[nameAsStr] = std::make_unique< ChirpUserData >(name);
    return m_userData[nameAsStr].get();
  }

  static std::unique_ptr< ChirpBinarySection > CreateUSRSection(u32 size, u32 elementCount,
                                                                u16 versionMajor, u16 versionMinor);
  static std::unique_ptr< ChirpBinarySection > CreateUSRSectionWithOffset(u64 offset, u32 size,
                                                                          u32 elementCount,
                                                                          u16 versionMajor,
                                                                          u16 versionMinor);

protected:
  s64 SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset = 0) override;

private:
  enum class ChunkId : u8 { KeyValues, Bools, S64s, Doubles, Strings, Tuples };
  enum class ValueType : u8 { Null, Bool, S64, Double, String, Tuple };

  void ReadOffsets(SectionReader& sectionReader);
  void ReadUserData(SectionReader& sectionReader, ChirpUserData* parentUserData);
  void ReadKeyValue(SectionReader& sectionReader, ChirpUserData* ownerUserData);
  void ReadTuples(SectionReader& sectionReader, ChirpKeyValue* parentKeyValue, u16 count);

  u32 m_keyValuesOffset{0};
  u32 m_boolsOffset{0};
  u32 m_s64sOffset{0};
  u32 m_doublesOffset{0};
  u32 m_stringsOffset{0};
  u32 m_tuplesOffset{0};

  std::unordered_map< std::string, std::unique_ptr< ChirpUserData > > m_userData;
};
} // namespace chirp::binary
