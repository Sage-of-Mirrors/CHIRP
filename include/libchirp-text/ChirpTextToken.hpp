#pragma once
#include "ChirpTextSource.hpp"
#include <string>

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
  Newline
};

struct ChirpTextToken {
  ChirpTextTokenKind mKind;
  std::string mText;
  ChirpTextSourceSpan mSpan;
};

const char* ChirpTextTokenKindName(ChirpTextTokenKind kind);
