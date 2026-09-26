#pragma once
#include "libchirp-text/ChirpTextDocument.hpp"
#include <optional>
#include <string>

namespace example_game {

struct Properties {
  std::optional<std::int64_t> collisionLayer;
  std::optional<bool> dynamic;
  std::optional<double> mass;
};

inline Properties ReadGameUserData(const chirp::text::ChirpTextDocument& document) {
  Properties out;

  for (const auto& ns : document.mUserData) {
    if (ns.mNamespaceName != "game")
      continue;

    for (const auto& field : ns.mFields) {
      if (field.mName == "collision_layer" && std::holds_alternative<std::int64_t>(field.mValue.mData))
        out.collisionLayer = std::get<std::int64_t>(field.mValue.mData);

      else if (field.mName == "dynamic" && std::holds_alternative<bool>(field.mValue.mData))
        out.dynamic = std::get<bool>(field.mValue.mData);

      else if (field.mName == "mass" && std::holds_alternative<double>(field.mValue.mData))
        out.mass = std::get<double>(field.mValue.mData);
    }
  }

  return out;
}

} // namespace example_game
