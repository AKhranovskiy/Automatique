#include "catch2/catch.hpp"

#include "position.hpp"
#include "world.h"

namespace {
using pos_t = position_t<5, 5>;
} // namespace

TEST_CASE("Positions are", "[position]") {
  SECTION("equal") {
    CHECK(pos_t{1, 1} == pos_t{6, 6});
    CHECK(pos_t{1, 6} == pos_t{6, 1});
    CHECK(pos_t{1, 2} == pos_t{1, 2});
  }
  SECTION("not equal") { CHECK(pos_t{1, 1} != pos_t{2, 2}); }
  SECTION("less") {
    CHECK(pos_t{1, 1} < pos_t{1, 2});
    CHECK(pos_t{1, 1} < pos_t{2, 1});
    CHECK(pos_t{1, 1} < pos_t{2, 2});
  }
  SECTION("greater") {
    CHECK(pos_t{2, 3} > pos_t{1, 2});
    CHECK(pos_t{2, 3} > pos_t{2, 2});
  }
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

TEST_CASE("Neighbors", "[position]") {
  const auto compare_arrays = [](const auto& lhs, const auto& rhs) {
    if (lhs.size() != rhs.size()) return false;
    for (const auto& e : lhs)
      if (std::find(rhs.cbegin(), rhs.cend(), e) == rhs.cend()) return false;
    return true;
  };
  using nbrs_t = decltype(neighborhs(pos_t{}));

  REQUIRE(compare_arrays(neighborhs(pos_t{2, 2}),
                         nbrs_t{pos_t{2, 1}, pos_t{3, 2}, pos_t{2, 3}, pos_t{1, 2}}));
}
