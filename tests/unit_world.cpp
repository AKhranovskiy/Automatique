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
    CHECK(areas.has(move(center, EDirection::North)));
    CHECK(areas.has(move(center, EDirection::East)));
    CHECK(areas.has(move(center, EDirection::South)));
    CHECK(areas.has(move(center, EDirection::West)));
  }
}
