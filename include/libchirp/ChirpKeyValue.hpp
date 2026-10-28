#pragma once

#include "libchirp/ChirpCommon.hpp"

#include <optional>
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
  ChirpKeyValue(const std::string& key) : m_key(key), m_value(nullptr) {}
  ChirpKeyValue() : ChirpKeyValue("") {}
  ~ChirpKeyValue();

  // Main get template. If the underlying value's type matches the desired type, returns the value.
  // Otherwise, returns nullopt.
  template < typename T >
  std::optional< T > Get() {
    T* valPtr = std::get_if< T >(&m_value);
    return valPtr ? std::optional< T >(*valPtr) : std::nullopt;
  }

  // Get() specialization for enum types. If the underlying value's type is s64, returns that value
  // cast to the desired enum type. Otherwise, returns nullopt.
  template < typename T >
  std::optional< T > Get()
    requires std::is_enum_v< T >
  {
    s64* valPtr = std::get_if< s64 >(&m_value);
    return valPtr ? std::optional< T >(static_cast< T >(*valPtr)) : std::nullopt;
  }

  // Get() specialization for std::filesystem::path. If the underlying value's type is string,
  // returns a path constructed from that string. Otherwise, returns nullopt.
  template <>
  std::optional< std::filesystem::path > Get() {
    std::string* valPtr = std::get_if< std::string >(&m_value);
    return valPtr ? std::optional< std::filesystem::path >(std::filesystem::path(*valPtr))
                  : std::nullopt;
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

  bool IsTuple();
  bool HasMember(const std::string& key);
  ChirpKeyValue& GetMember(const std::string& key);

  ChirpKeyValue& operator[](const std::string& key);

private:
  std::string m_key;
  ChirpKeyValueVariant m_value;
};
} // namespace chirp
