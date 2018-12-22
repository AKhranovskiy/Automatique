#pragma once

#include "chronicles.h"
#include "types/position.hpp"
#include <unordered_set>

struct World {
  static constexpr const coord_t WIDTH{5u};
  static constexpr const coord_t HEIGHT{5u};

  using position_t = position_t<WIDTH, HEIGHT>;
  using position_hash_t = position_hash<WIDTH, HEIGHT>;
  using path_t = path_t<WIDTH, HEIGHT>;

  using tiles_t = std::unordered_map<position_t, ETileContent, position_hash_t>;
  static tiles_t Tiles;

  static ETileContent get_tile_info(position_t position) noexcept;

  using area_list_t = std::vector<World::position_t>;
  static area_list_t get_areas(const World::position_t& center, distance_t radius,
                               ETileContent tile) noexcept;
  static area_list_t get_areas(ETileContent tile) noexcept;

  static chronicles_t& Chronicles() noexcept;
  static chronicles_t& Chronicles(chronicles_t& new_chronicles) noexcept;
};
