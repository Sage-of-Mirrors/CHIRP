#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/section/ChirpBinarySection.hpp"

#include <variant>

namespace chirp::binary {
class ChirpKeyValue {
public:
  using ChirpKeyValueTuple = std::vector< ChirpKeyValue >;
  using ChirpUserValue = std::variant< std::nullptr_t, bool, s64, double, std::string,
                                       ChirpKeyValueTuple, ChirpKeyValue* >;

  ChirpKeyValue() : m_value(nullptr) {}

  explicit ChirpKeyValue(std::string_view key, ChirpUserValue value)
  : m_key(key), m_value(std::move(value)) {}

  std::string GetKey() const { return m_key; }
  const ChirpUserValue& GetValue() const { return m_value; }

  template < typename T >
  std::optional< const T > GetValueAs() const {
    return std::holds_alternative< T >(m_value) ? std::optional< T >(std::get< T >(m_value))
                                                : std::nullopt;
  }

  std::optional< const ChirpKeyValue > GetTupleValue(std::string_view key) const;
  std::optional< const ChirpKeyValue > operator[](std::string_view key) const;

private:
  std::string m_key;
  ChirpUserValue m_value;
};

class ChirpUserData {
public:
  ChirpUserData() {}
  ChirpUserData(std::string_view namesp) : m_namespace(namesp) {}

  std::string GetNamespace() const { return m_namespace; }
  std::unordered_map< std::string, ChirpKeyValue >& GetFields() { return m_fields; }
  std::vector< ChirpUserData >& GetChildren() { return m_children; }

  const ChirpKeyValue& GetField(std::string_view key) const {
    std::string keyAsStr(key);
    return m_fields.contains(keyAsStr) ? m_fields.at(keyAsStr) : ChirpKeyValue();
  }

  template < typename T >
  const T GetFieldAs(std::string_view key) const {
    std::string keyAsStr(key);
    if (!m_fields.contains(keyAsStr)) {
      return T();
    }

    return m_fields.at(keyAsStr).GetValueAs< T >().value_or(T());
  }

  bool AddField(std::string_view key, ChirpKeyValue&& value) {
    std::string keyAsStr(key);
    if (m_fields.contains(keyAsStr)) {
      return false;
    }

    m_fields[keyAsStr] = std::move(value);
    return true;
  }

  std::optional< const ChirpKeyValue > operator[](std::string_view key) const;

private:
  std::string m_namespace;
  std::unordered_map< std::string, ChirpKeyValue > m_fields;
  std::vector< ChirpUserData > m_children;
};

class ChirpBinaryUserdata : public ChirpBinarySection {
public:
  ChirpBinaryUserdata();
  ChirpBinaryUserdata(u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);
  ChirpBinaryUserdata(u64 offset, u32 size, u32 elementCount, u16 versionMajor, u16 versionMinor);

  bool Import(SectionReader& inStream) override;
  bool Export(athena::io::IStreamWriter& outStream) override;

  ChirpUserData* GetUserData() const { return m_userData.get(); }

  static std::unique_ptr< ChirpBinarySection > CreateUSRSection(u32 size, u32 elementCount,
                                                                u16 versionMajor, u16 versionMinor);
  static std::unique_ptr< ChirpBinarySection > CreateUSRSectionWithOffset(u64 offset, u32 size,
                                                                          u32 elementCount,
                                                                          u16 versionMajor,
                                                                          u16 versionMinor);

protected:
  s64 SeekToChunk(SectionReader& sectionReader, u8 chunkId, u32 dataOffset = 0) override;

private:
  enum class ChunkId : u8 { Attributes };
  enum class KeyValueType : u16 { Null, Bool, S64, Double, String, Tuple };

  std::unique_ptr< ChirpUserData > m_userData;
};
} // namespace chirp::binary
