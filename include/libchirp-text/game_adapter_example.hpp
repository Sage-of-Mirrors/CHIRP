#pragma once
#include "chirp/ir/text/AST.hpp"
#include <optional>
#include <string>

namespace example_game {

struct Properties {
  std::optional< std::int64_t > collisionLayer;
  std::optional< bool > dynamic;
  std::optional< double > mass;
};

inline Properties readGameUserData(const chirp::ir::text::Document& document) {
  Properties out;

  for (const auto& ns : document.userData) {
    if (ns.namespaceName != "game")
      continue;

    for (const auto& field : ns.fields) {
      if (field.name == "collision_layer" &&
          std::holds_alternative< std::int64_t >(field.value.data))
        out.collisionLayer = std::get< std::int64_t >(field.value.data);

      else if (field.name == "dynamic" && std::holds_alternative< bool >(field.value.data))
        out.dynamic = std::get< bool >(field.value.data);

      else if (field.name == "mass" && std::holds_alternative< double >(field.value.data))
        out.mass = std::get< double >(field.value.data);
    }
  }

  return out;
}

} // namespace example_game
