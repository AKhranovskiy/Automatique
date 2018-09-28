#pragma once

#include "position.hpp"

enum class ETileContent {
  Unknown,
  None,
  Coal,
  Ore,
  Water,
};

struct World {
  static constexpr const coord_t WIDTH{5u};
  static constexpr const coord_t HEIGHT{5u};

  using position_t = position_t<WIDTH, HEIGHT>;
  using position_hash_t = position_hash<WIDTH, HEIGHT>;
  using path_t = path_t<WIDTH, HEIGHT>;

  using tiles_t = std::unordered_map<position_t, ETileContent, position_hash_t>;
  static tiles_t Tiles;
};

World::tiles_t World::Tiles{};
