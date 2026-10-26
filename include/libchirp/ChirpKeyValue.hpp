#pragma once

#include "libchirp/ChirpCommon.hpp"

#include <type_traits>
#include <unordered_map>
#include <variant>

namespace chirp {
class ChirpKeyValue;

using ChirpKeyValueTuple = std::unordered_map< std::string, ChirpKeyValue >;
using ChirpKeyValueVariant =
    std::variant< std::nullptr_t, bool, s64, double, std::string, ChirpKeyValueTuple* >;

class ChirpKeyValue {
public:
  ChirpKeyValue(std::string_view key) : m_key(key), m_value(nullptr) {}
  ChirpKeyValue() : ChirpKeyValue("") {}

  // Main get template. Returns the value as the given type if possible,
  // otherwise returns a default-constructed value.
  template < typename T >
  T Get() {
    T* valPtr = std::get_if< T >(&m_value);
    return valPtr ? *valPtr : T{};
  }

  // Get() specialization for enum types. Returns the value as an s64 cast to the
  // desired enum type.
  template < typename T >
  T Get()
    requires std::is_enum_v< T >
  {
    return static_cast< T >(Get< s64 >());
  }

  // Get() specialization for std::filesystem::path. Will return a path constructed
  // from the value as a string.
  template <>
  std::filesystem::path Get() {
    return std::filesystem::path(Get< std::string >());
  }

  // Main set template.
  template < typename T >
  void Set(const T& value) {
    m_value = value;
  }

  // Set() specialization for enum values. Given enum value is cast to s64.
  template < typename T >
  void Set(const T& value)
    requires std::is_enum_v< T >
  {
    m_value = static_cast< s64 >(value);
  }

  // Set() specialization for std::filesystem::path. Given path is converted
  // into a string.
  template <>
  void Set(const std::filesystem::path& value) {
    m_value = value.generic_string();
  }

private:
  std::string m_key;
  ChirpKeyValueVariant m_value;
};
} // namespace chirp
