#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/ChirpKeyValue.hpp"

namespace chirp::binary {
class ChirpBinaryIOOptions {
public:
  ChirpBinaryIOOptions() {}

  template < typename T >
  T Get(std::string_view key) {
    std::string keyAsStr(key);
    auto itr = m_options.find(keyAsStr);
    return itr != m_options.end() ? itr->second.Get< T >() : T{};
  }

  template < typename T >
  void Set(std::string_view key, const T& value) {
    std::string keyAsStr(key);
    m_options.emplace(keyAsStr, keyAsStr);
    m_options[keyAsStr].Set< T >(value);
  }

private:
  ChirpKeyValueTuple m_options;
};
} // namespace chirp::binary
