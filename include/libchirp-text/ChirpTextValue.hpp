#pragma once
#include "libchirp-common/ChirpErrorSource.hpp"
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace chirp::text {
struct ChirpTextValue {
  using ChirpTextTuple = std::vector<ChirpTextValue>;
  using ChirpTextData = std::variant<std::nullptr_t, bool, std::int64_t, double, std::string, ChirpTextTuple>;

  ChirpTextData mData;
  ChirpSourceSpan mSpan;

  ChirpTextValue()
  : mData(nullptr) {}
  explicit ChirpTextValue(ChirpTextData value, ChirpSourceSpan s = {})
  : mData(std::move(value))
  , mSpan(std::move(s)) {}
};
} // namespace chirp::text