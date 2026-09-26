#include "libchirp-text/ChirpTextToken.hpp"

const char* ChirpTextTokenKindName(ChirpTextTokenKind kind) {
  switch (kind) {
  case ChirpTextTokenKind::End:
    return "end of file";
  case ChirpTextTokenKind::Identifier:
    return "identifier";
  case ChirpTextTokenKind::Integer:
    return "integer";
  case ChirpTextTokenKind::Float:
    return "float";
  case ChirpTextTokenKind::String:
    return "string";
  case ChirpTextTokenKind::At:
    return "@";
  case ChirpTextTokenKind::Percent:
    return "%";
  case ChirpTextTokenKind::Hash:
    return "#";
  case ChirpTextTokenKind::LParen:
    return "(";
  case ChirpTextTokenKind::RParen:
    return ")";
  case ChirpTextTokenKind::LBrace:
    return "{";
  case ChirpTextTokenKind::RBrace:
    return "}";
  case ChirpTextTokenKind::Comma:
    return ",";
  case ChirpTextTokenKind::Equals:
    return "=";
  case ChirpTextTokenKind::Newline:
    return "newline";
  }
  return "unknown";
}
