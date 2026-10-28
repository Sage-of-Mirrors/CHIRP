#include "libchirp/ChirpKeyValue.hpp"

#include <stdexcept>

namespace {
// Copied from https://en.cppreference.com/cpp/utility/variant/visit2, visitor #4.
template < class... Ts >
struct KeyValueVisitor : Ts... {
  using Ts::operator()...;
};
} // namespace

namespace chirp {
ChirpKeyValueType ChirpKeyValue::GetType() {
  return std::visit(
      KeyValueVisitor{
          [](std::nullptr_t val) { return ChirpKeyValueType::Null; },
          [](bool val) { return ChirpKeyValueType::Bool; },
          [](s64 val) { return ChirpKeyValueType::S64; },
          [](double val) { return ChirpKeyValueType::Double; },
          [](const std::string& val) { return ChirpKeyValueType::String; },
          [](std::shared_ptr< ChirpKeyValueTuple > val) { return ChirpKeyValueType::Tuple; }},
      m_value);
}

u16 ChirpKeyValue::GetSize() {
  return std::visit(
      KeyValueVisitor{[](auto val) -> u16 { return 0; },
                      [](const std::string& val) { return static_cast< u16 >(val.size() + 1); },
                      [](SharedChirpKeyValueTuple val) { return static_cast< u16 >(val->size()); }},
      m_value);
}

bool ChirpKeyValue::IsTuple() {
  auto tupleOpt = Get< SharedChirpKeyValueTuple >();
  if (!tupleOpt.has_value() || tupleOpt.value() == nullptr) {
    return false;
  }

  return true;
}

bool ChirpKeyValue::HasMember(const std::string& key) {
  auto tupleOpt = Get< SharedChirpKeyValueTuple >();
  if (!tupleOpt.has_value() || tupleOpt.value() == nullptr) {
    return false;
  }

  return tupleOpt.value()->contains(key);
}

ChirpKeyValue& ChirpKeyValue::GetMember(const std::string& key) {
  auto tupleOpt = Get< SharedChirpKeyValueTuple >();
  if (!tupleOpt.has_value() || tupleOpt.value() == nullptr) {
    throw std::invalid_argument("Attempted to access member from ChirpKeyValue instance that was "
                                "not holding a tuple type.");
  }

  ChirpKeyValueTuple& tuple = *tupleOpt.value();
  if (!tuple.contains(key)) {
    throw std::invalid_argument(
        "Attempted to access member from ChirpKeyValue tuple that did not have that member.");
  }

  return tuple[key];
}

ChirpKeyValue& ChirpKeyValue::operator[](const std::string& key) { return GetMember(key); }
} // namespace chirp