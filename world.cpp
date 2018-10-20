#include "world.h"

World::tiles_t World::Tiles{};

namespace {
using area_set_t = std::unordered_set<World::position_t, World::position_hash_t>;

void gather_areas(const World::position_t& center, distance_t radius, ETileContent tile,
                  const World::position_t& pos, area_set_t& result) noexcept {
  const auto check_tile = [tile](const World::position_t& pos) {
    if (tile == ETileContent::Unknown && World::Tiles.count(pos) == 0) return true;
    return World::Tiles.count(pos) == 1 && World::Tiles.at(pos) == tile;
  };
  if (result.count(pos) == 0 && distance(center, pos) <= radius && check_tile(pos)) {
    result.insert(pos);
    for (const auto& t : neighborhs(pos)) gather_areas(center, radius, tile, t, result);
  }
}
} // namespace

World::area_list_t World::get_areas(const World::position_t& center, distance_t radius,
                                    ETileContent tile) noexcept {
  area_set_t result;
  gather_areas(center, radius, tile, center, result);
  return {result.begin(), result.end()};
}
