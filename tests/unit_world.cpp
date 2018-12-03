#include "catch2/catch.hpp"

#include "world.h"

TEST_CASE("Tile Content Count", "[world]") { REQUIRE(5 == get_tile_content_count()); }

TEST_CASE("Find unknown areas", "[world]") {
  const auto center = World::position_t{2, 2};
  const auto unknown_tile = ETileContent::Unknown;

  const auto get_areas = [center, tile = unknown_tile](distance_t d) {
    struct {
      const World::area_list_t areas;
      inline constexpr bool has(World::position_t pos) const noexcept {
        return std::count(areas.cbegin(), areas.cend(), pos) != 0;
      }
      inline constexpr bool has(std::initializer_list<World::position_t>&& positions) const
          noexcept {
        return std::all_of(positions.begin(), positions.end(),
                           [this](const auto& pos) { return this->has(pos); });
      }
      inline constexpr size_t size() const noexcept { return areas.size(); }
    } areas{World::get_areas(center, d, tile)};
    return areas;
  };

  SECTION("within zero radius") {
    const auto areas = get_areas(0);
    CHECK(areas.size() == 1);
    CHECK(areas.has(center));
  }
  SECTION("within one radius") {
    const auto areas = get_areas(1);
    CHECK(areas.size() == 5);
    CHECK(areas.has(center));
    CHECK(areas.has({{1, 2}, {3, 2}, {2, 1}, {2, 3}}));
  }
  SECTION("within radius 2") {
    const auto areas = get_areas(2);
    CHECK(areas.size() == 13);
    CHECK(areas.has(center));
    CHECK(areas.has({{1, 2}, {3, 2}, {2, 1}, {2, 3}}));
    CHECK(areas.has({{1, 1}, {3, 1}, {1, 3}, {3, 3}}));
    CHECK(areas.has({{2, 0}, {0, 2}, {4, 2}, {2, 4}}));
  }
  SECTION("within radius 3") {
    const auto areas = get_areas(3);
    CHECK(areas.size() == 21);
    CHECK(areas.has(center));
    CHECK(areas.has({{1, 2}, {3, 2}, {2, 1}, {2, 3}}));
    CHECK(areas.has({{1, 1}, {3, 1}, {1, 3}, {3, 3}}));
    CHECK(areas.has({{2, 0}, {0, 2}, {4, 2}, {2, 4}}));
  }
  SECTION("within radius 4") {
    const auto areas = get_areas(4);
    CHECK(areas.size() == 25);
    CHECK(areas.has(center));
    CHECK(areas.has({{1, 2}, {3, 2}, {2, 1}, {2, 3}}));
    CHECK(areas.has({{1, 1}, {3, 1}, {1, 3}, {3, 3}}));
    CHECK(areas.has({{2, 0}, {0, 2}, {4, 2}, {2, 4}}));
  }
  SECTION("within overlapping radius") { CHECK(get_areas(5).size() == 25); }
}

TEST_CASE("Find areas", "[world]") {
  const auto center = World::position_t{2, 2};
  const auto radius = distance_t{4};

  const auto check_areas = [&center](ETileContent tile,
                                     std::initializer_list<World::position_t>&& expected) {
    auto areas = World::get_areas(center, radius, tile);
    return areas.size() == expected.size() &&
           std::all_of(expected.begin(), expected.end(), [&areas](const auto& pos) {
             return std::count(areas.cbegin(), areas.cend(), pos) != 0;
           });
  };

  struct WorldState {
    WorldState() {
      const auto get_tile = [](World::position_t pos) -> ETileContent {
        const auto even = [](auto v) { return (v % 2) == 0; };
        const auto odd = [](auto v) { return (v % 2) != 0; };
        if (even(pos.x) && even(pos.y)) return ETileContent::None;
        if (odd(pos.x) && even(pos.y)) return ETileContent::Coal;
        if (even(pos.x) && odd(pos.y)) return ETileContent::Ore;
        if (odd(pos.x) && odd(pos.y)) return ETileContent::Water;
        return ETileContent::Unknown;
      };

      for (auto x = 0u; x < World::WIDTH; ++x) {
        for (auto y = 0u; y < World::HEIGHT; ++y) {
          World::position_t pos{x, y};
          World::Tiles.emplace(pos, get_tile(pos));
        }
      }
    }
    ~WorldState() { World::Tiles.clear(); }
  };
  WorldState _{};

  CHECK(check_areas(ETileContent::Ore, {{0, 1}, {0, 3}, {2, 1}, {2, 3}, {4, 1}, {4, 3}}));
  CHECK(check_areas(ETileContent::None,
                    {{0, 0}, {0, 2}, {0, 4}, {2, 0}, {2, 2}, {2, 4}, {4, 0}, {4, 2}, {4, 4}}));
  CHECK(check_areas(ETileContent::Water, {{1, 1}, {1, 3}, {3, 1}, {3, 3}}));
  CHECK(check_areas(ETileContent::Coal, {{1, 0}, {1, 2}, {1, 4}, {3, 0}, {3, 2}, {3, 4}}));
}
