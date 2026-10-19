#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/section/ChirpBinarySection.hpp"

#include <variant>

namespace chirp::binary {
class ChirpKeyValue;

using ChirpUserTuple = std::unordered_map< std::string, ChirpKeyValue >;
using ChirpUserValue =
    std::variant< std::nullptr_t, bool, s64, double, std::string, ChirpUserTuple >;

class ChirpKeyValue {
public:
  ChirpKeyValue(std::string_view key) : m_key(key), m_value(nullptr) {}
  ChirpKeyValue() : m_key(""), m_value(nullptr) {}

  std::string GetKey() const { return m_key; }
  const ChirpUserValue& GetValue() const { return m_value; }

  template < typename T >
  const T& GetValue() const {
    const T* valPtr = std::get_if< T >(&m_value);
    if (!valPtr) {
      throw new std::invalid_argument("UserValue value was empty.");
    }

    return *valPtr;
  }

  template < typename T >
  T& GetValue() {
    T* valPtr = std::get_if< T >(&m_value);
    if (!valPtr) {
      throw new std::invalid_argument("UserValue value was empty.");
    }

    return *valPtr;
  }

  const ChirpKeyValue& GetValue(std::string_view key);

  template < typename T >
  void SetValue(const T& newVal) {
    m_value = newVal;
  }

  void SetValue(ChirpUserTuple&& tuple) { m_value = tuple; }

  const ChirpKeyValue& operator[](std::string_view key);

private:
  std::string m_key;
  ChirpUserValue m_value;
};

class ChirpUserData {
public:
  ChirpUserData(std::string_view name) : m_namespace(name) {}

  std::string GetNamespace() const { return m_namespace; }
  std::unordered_map< std::string, std::unique_ptr< ChirpKeyValue > >& GetFields() {
    return m_fields;
  }
  std::unordered_map< std::string, std::unique_ptr< ChirpUserData > >& GetChildren() {
    return m_children;
  }

  bool HasField(std::string_view key) const { return m_fields.contains(std::string(key)); }

  const ChirpKeyValue& GetField(std::string_view key);

  template < typename T >
  const T& GetValue(std::string_view key) {
    if (!HasField(key)) {
      throw new std::invalid_argument("KeyValue key not found.");
    }

    return m_fields[std::string(key)]->GetValue< T >();
  }

  ChirpUserData* CreateChild(std::string_view name);
  ChirpKeyValue* CreateField(std::string_view name);

  const ChirpKeyValue& operator[](std::string_view key);

private:
  std::string m_namespace;
  std::unordered_map< std::string, std::unique_ptr< ChirpKeyValue > > m_fields;
  std::unordered_map< std::string, std::unique_ptr< ChirpUserData > > m_children;
};

class ChirpBinaryUserdata : public ChirpBinarySection {
public:
  ChirpBinaryUserdata();
  ChirpBinaryUserdata(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);
  ChirpBinaryUserdata(u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);

  bool Import(SectionReader& inStream) override;
  bool Export(athena::io::IStreamWriter& outStream) override;

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
