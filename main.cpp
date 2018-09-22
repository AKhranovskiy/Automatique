#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

namespace cx {

template <char ch>
constexpr size_t to_num()
{
  if constexpr (ch == ' ') {
    return 0u;
  }
  static_assert('0' <= ch && ch <= '9', "Character is not a number");
  return ch - '0';
}
constexpr size_t year()
{
  constexpr const char* const s = __DATE__ + 7;
  return to_num<s[0]>() * 1000 + to_num<s[1]>() * 100 + to_num<s[2]>() * 10 + to_num<s[3]>();
}
template <char a, char b, char c>
struct month_t;

template <>
struct month_t<'S', 'e', 'p'> {
  static const size_t id = 9u;
};

constexpr size_t month()
{
  constexpr const char* const s = __DATE__;
  return month_t<s[0], s[1], s[2]>::id;
}
constexpr size_t day()
{
  constexpr const char* const s = __DATE__ + 4;
  return to_num<s[0]>() * 10 + to_num<s[1]>();
}

constexpr size_t date()
{
  return year() * 10000 + month() * 100 + day();
};

} // namespace cx

struct version_t {
  size_t major{0};
  size_t minor{0};
  size_t date{cx::date()};
  const char* const name{"Automatique"};
};

std::ostream& operator<<(std::ostream& os, const version_t& v) noexcept
{
  return os << v.name << " v" << v.major << '.' << v.minor << '-' << v.date;
}

constexpr version_t KVersion = {0, 1};

enum class EDirection {
  North,
  East,
  South,
  West
};

using coord_t = size_t;

template <coord_t W, coord_t H>
struct position_t {
  coord_t x{0};
  coord_t y{0};

  explicit position_t(coord_t x, coord_t y) noexcept
      : x{x % W}
      , y{y % H}
  {
  }

  position_t move(EDirection dir) const noexcept
  {
    switch (dir) {
    case EDirection::North:
      return {x, y > 0 ? y - 1 : H - 1};
    case EDirection::East:
      return {x + 1, y};
    case EDirection::South:
      return {x, y + 1};
    case EDirection::West:
      return {x > 0 ? x - 1 : W - 1};
    }
  }
};

template <coord_t W, coord_t H>
struct world_t {
  using position_t = position_t<W, H>;

  using distance_t = size_t;
  distance_t distance(position_t from, position_t to) const
  {
    const auto dst = [](auto a, auto b) {
      return a > b ? a - b : b - a;
    };

    auto dx = std::min(dst(from.x, to.x), dst(from.x + W, to.x));
    auto dy = std::min(dst(from.y, to.y), dst(from.y + H, to.y));
    return dx * dx + dy * dy;
  }

  // std::vector<position_t> findPath(position_t start, position_t goal) const
  // {
  // }

private:
};

/*
function reconstruct_path(cameFrom, current)
    total_path := {current}
    while current in cameFrom.Keys:
        current := cameFrom[current]
        total_path.append(current)
    return total_path

function A_Star(start, goal)
    // The set of nodes already evaluated
    closedSet := {}

    // The set of currently discovered nodes that are not evaluated yet.
    // Initially, only the start node is known.
    openSet := {start}

    // For each node, which node it can most efficiently be reached from.
    // If a node can be reached from many nodes, cameFrom will eventually contain the
    // most efficient previous step.
    cameFrom := an empty map

    // For each node, the cost of getting from the start node to that node.
    gScore := map with default value of Infinity

    // The cost of going from start to start is zero.
    gScore[start] := 0

    // For each node, the total cost of getting from the start node to the goal
    // by passing by that node. That value is partly known, partly heuristic.
    fScore := map with default value of Infinity

    // For the first node, that value is completely heuristic.
    fScore[start] := heuristic_cost_estimate(start, goal)

    while openSet is not empty
        current := the node in openSet having the lowest fScore[] value
        if current = goal
            return reconstruct_path(cameFrom, current)

        openSet.Remove(current)
        closedSet.Add(current)

        for each neighbor of current
            if neighbor in closedSet
                continue		// Ignore the neighbor which is already evaluated.

            // The distance from start to a neighbor
            tentative_gScore := gScore[current] + dist_between(current, neighbor)

            if neighbor not in openSet	// Discover a new node
                openSet.Add(neighbor)
            else if tentative_gScore >= gScore[neighbor]
                continue		// This is not a better path.

            // This path is the best until now. Record it!
            cameFrom[neighbor] := current
            gScore[neighbor] := tentative_gScore
            fScore[neighbor] := gScore[neighbor] + heuristic_cost_estimate(neighbor, goal)
            */
int main()
{
  std::cout << KVersion << '\n';
  std::cout << "Start game loop" << std::endl;

  const auto world = world_t<5, 5>{};
  using pos_t = decltype(world)::position_t;

  assert(2 == world.distance(pos_t{0, 0}, pos_t{1, 1}));
  assert(2 == world.distance(pos_t{0, 0}, pos_t{4, 4}));
  assert(1 == world.distance(pos_t{0, 0}, pos_t{4, 0}));

  // bool quitRequested = false;
  // while (!quitRequested) {
  //   while (std::string::traits_type::not_eof(std::cin.peek())) {
  //     if (std::cin.get() == 'q') {
  //       std::cout << "Quit\n";
  //       quitRequested = true;
  //       break;
  //     }
  //   }
  // }

  std::cout << "Finish game loop\n"
            << std::endl;
  return 0;
}
