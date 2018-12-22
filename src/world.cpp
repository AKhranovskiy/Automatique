#include "world.h"
#include <random>

World::tiles_t World::Tiles{};

namespace {

chronicles_t& GetDefaultChronicles() noexcept {
  static chronicles_t chronicles{};
  return chronicles;
}
chronicles_t* CurrentChronicles = &GetDefaultChronicles();
} // namespace

ETileContent World::get_tile_info(position_t position) noexcept {
  if (World::Tiles.count(position) == 0) {
    static auto tile_gen = []() -> ETileContent {
      static std::random_device rd;
      static std::ranlux24 gen = []() {
        std::ranlux24 e{rd()};
        e.seed(44);
        return e;
      }();
      static_assert(get_tile_content_count() == 5);
      static std::discrete_distribution<> dis({0, 88, 4, 4, 4});
      return static_cast<ETileContent>(dis(gen));
    };
    const auto& [it, r] = World::Tiles.emplace(position, tile_gen());
    const auto& [p, tile] = *it;
    return tile;
  }

  return World::Tiles.at(position);
}

World::area_list_t World::get_areas(const World::position_t& center, distance_t radius,
                                    ETileContent tile) noexcept {
  const auto check_tile = [tile](const World::position_t& pos) {
    if (tile == ETileContent::Unknown && World::Tiles.count(pos) == 0) return true;
    return World::Tiles.count(pos) == 1 && World::Tiles.at(pos) == tile;
  };

  using area_set_t = std::unordered_set<World::position_t, World::position_hash_t>;
  area_set_t result;

  area_set_t queue{center};
  area_set_t visited{};

  while (!queue.empty()) {
    auto current = *queue.begin();
    queue.erase(queue.begin());
    if (visited.insert(current).second && distance(center, current) <= radius) {
      if (check_tile(current)) result.insert(current);
      auto nhs = neighborhs(current);
      queue.insert(nhs.begin(), nhs.end());
    }
  }

  return {result.begin(), result.end()};
}

World::area_list_t World::get_areas(ETileContent tile) noexcept {
  area_list_t result;
  for (const auto& [pos, cont] : World::Tiles) {
    if (cont == tile) result.push_back(pos);
  }
  return result;
}

chronicles_t& World::Chronicles() noexcept { return *CurrentChronicles; }
chronicles_t& World::Chronicles(chronicles_t& new_chronicles) noexcept {
  CurrentChronicles = &new_chronicles;
  return Chronicles();
}
