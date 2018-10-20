#pragma once

#include "position.hpp"
#include <unordered_set>

enum class ETileContent {
  Unknown,
  None,
  Coal,
  Ore,
  Water,
};
constexpr std::underlying_type_t<ETileContent> get_tile_content_count() noexcept {
  std::underlying_type_t<ETileContent> count = 0;
  switch (ETileContent{}) {
  case ETileContent::Unknown:
    ++count;
    [[fallthrough]];
  case ETileContent::None:
    ++count;
    [[fallthrough]];
  case ETileContent::Coal:
    ++count;
    [[fallthrough]];
  case ETileContent::Ore:
    ++count;
    [[fallthrough]];
  case ETileContent::Water:
    ++count;
    [[fallthrough]];
  default: {};
  }
  return count;
}

struct World {
  static constexpr const coord_t WIDTH{5u};
  static constexpr const coord_t HEIGHT{5u};

  using position_t = position_t<WIDTH, HEIGHT>;
  using position_hash_t = position_hash<WIDTH, HEIGHT>;
  using path_t = path_t<WIDTH, HEIGHT>;

  using tiles_t = std::unordered_map<position_t, ETileContent, position_hash_t>;
  static tiles_t Tiles;

  using area_list_t = std::vector<World::position_t>;
  static area_list_t get_areas(const World::position_t& center, distance_t radius,
                               ETileContent tile) noexcept;
};
