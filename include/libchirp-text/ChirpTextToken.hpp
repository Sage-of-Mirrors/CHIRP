#pragma once
#include "libchirp/ChirpErrorSource.hpp"
#include <string>
namespace chirp::text {
enum class ChirpTextTokenKind {
  End,
  Identifier,
  Integer,
  Float,
  String,
  At,
  Percent,
  Hash,
  LParen,
  RParen,
  LBrace,
  RBrace,
  Comma,
  Equals,
  Newline,
};

struct ChirpTextToken {
  ChirpTextTokenKind mKind;
  std::string mText;
  ChirpSourceSpan mSpan;
};

const char* ChirpTextTokenKindName(ChirpTextTokenKind kind);
} // namespace chirp::text
