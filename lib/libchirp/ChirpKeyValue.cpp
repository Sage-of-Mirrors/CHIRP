#include "libchirp/ChirpKeyValue.hpp"

#include <stdexcept>

namespace chirp {
ChirpKeyValue::~ChirpKeyValue() {
  auto tupleOpt = Get< ChirpKeyValueTuple* >();
  if (!tupleOpt.has_value() || tupleOpt.value() == nullptr) {
    return;
  }

  delete tupleOpt.value();
}

bool ChirpKeyValue::IsTuple() {
  auto tupleOpt = Get< ChirpKeyValueTuple* >();
  if (!tupleOpt.has_value() || tupleOpt.value() == nullptr) {
    return false;
  }

  return true;
}

bool ChirpKeyValue::HasMember(const std::string& key) {
  auto tupleOpt = Get< ChirpKeyValueTuple* >();
  if (!tupleOpt.has_value() || tupleOpt.value() == nullptr) {
    return false;
  }

  return tupleOpt.value()->contains(key);
}

ChirpKeyValue& ChirpKeyValue::GetMember(const std::string& key) {
  auto tupleOpt = Get< ChirpKeyValueTuple* >();
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