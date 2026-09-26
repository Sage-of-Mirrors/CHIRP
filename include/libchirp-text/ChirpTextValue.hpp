#pragma once
#include "libchirp-text/ChirpTextSource.hpp"
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

struct ChirpTextValue {
  using ChirpTextTuple = std::vector< ChirpTextValue >;
  using ChirpTextData = std::variant< std::nullptr_t, bool, std::int64_t, double, std::string, ChirpTextTuple >;

  ChirpTextData mData;
  ChirpTextSourceSpan mSpan;

  ChirpTextValue() : mData(nullptr) {}
  explicit ChirpTextValue(ChirpTextData value, ChirpTextSourceSpan s = {}) : mData(std::move(value)), mSpan(std::move(s)) {}
};