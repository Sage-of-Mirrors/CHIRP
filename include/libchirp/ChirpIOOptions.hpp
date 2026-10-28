#pragma once

#include "libchirp/ChirpCommon.hpp"
#include "libchirp/ChirpKeyValue.hpp"

namespace chirp::binary {
class ChirpBinaryIOOptions {
public:
  ChirpBinaryIOOptions() {}

  template < typename T >
  T Get(const std::string& key) {
    auto itr = m_options.find(key);
    return itr != m_options.end() ? itr->second.Get< T >() : T{};
  }

  template < typename T >
  void Set(const std::string& key, const T& value) {
    m_options.emplace(key, key);
    m_options[key].Set< T >(value);
  }

private:
  ChirpKeyValueTuple m_options;
};
} // namespace chirp::binary
