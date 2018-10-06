#include "catch2/catch.hpp"

#include "position.hpp"

namespace {
using pos_t = position_t<5, 5>;
}
TEST_CASE("Distance", "[position]") {
  const auto test_distance = [](const pos_t& a, const pos_t& b, distance_t d) {
    CHECK(distance(a, b) == d);
    CHECK(distance(b, a) == d);
  };

  SECTION("between the same point") { REQUIRE(0 == distance(pos_t{1, 1}, pos_t{1, 1})); }
  SECTION("between neighbor points") {
    constexpr const auto pos = pos_t{0, 0};
    test_distance(pos, move(pos, EDirection::North), 1);
    test_distance(pos, move(pos, EDirection::East), 1);
    test_distance(pos, move(pos, EDirection::South), 1);
    test_distance(pos, move(pos, EDirection::West), 1);
  }
  SECTION("between diagonal points") {
    constexpr const auto pos = pos_t{0, 0};
    const auto move2 = [&pos](EDirection a, EDirection b) { return move(move(pos, a), b); };

    test_distance(pos, move2(EDirection::North, EDirection::East), 2);
    test_distance(pos, move2(EDirection::East, EDirection::South), 2);
    test_distance(pos, move2(EDirection::South, EDirection::West), 2);
    test_distance(pos, move2(EDirection::West, EDirection::North), 2);
  }
}
