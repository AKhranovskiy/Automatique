#include <array>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

template <size_t N>
constexpr uint64_t FNVhash(const uint8_t (&data)[N])
{
  constexpr std::uint64_t basis = 0xcbf29ce484222325;
  constexpr std::uint64_t prime = 0x100000001b3;
  uint64_t hash = basis;
  for (auto i = 0u; i < N; ++i) {
    hash ^= data[i];
    hash *= prime;
  }
  return hash;
}

template <typename T>
constexpr std::uint64_t FNVhash(T a, T b)
{
  typedef uint8_t(data_t)[sizeof(T) * 2];

  const T buf[2] = {a, b};
  const data_t& ref = *reinterpret_cast<const data_t*>(&buf);
  return FNVhash(ref);
}
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

  position_t() = default;

  position_t(coord_t x, coord_t y) noexcept
      : x{x % W}
      , y{y % H}
  {
  }

  position_t(const position_t& pos) noexcept
      : x{pos.x}
      , y{pos.y}
  {
  }
  position_t(position_t&& pos) noexcept
      : x{pos.x}
      , y{pos.y}
  {
  }

  position_t& operator=(const position_t& pos) noexcept
  {
    x = pos.x;
    y = pos.y;
    return *this;
  }
  position_t move(EDirection dir) const noexcept
  {
    switch (dir) {
    case EDirection::North:
      return position_t{x, y > 0 ? y - 1 : H - 1};
    case EDirection::East:
      return position_t{x + 1, y};
    case EDirection::South:
      return position_t{x, y + 1};
    case EDirection::West:
      return position_t{x > 0 ? x - 1 : W - 1, y};
    }
  }

  struct Hash {
    std::size_t operator()(const position_t& p) const noexcept
    {
      return cx::FNVhash(p.x, p.y);
    }
  };

  bool operator==(const position_t& o) const noexcept
  {
    return x == o.x && y == o.y;
  }
};

template <coord_t W, coord_t H>
std::ostream& operator<<(std::ostream& os, const position_t<W, H>& pos)
{
  return (os << '(' << pos.x << ',' << pos.y << ')');
}

template <coord_t W, coord_t H>
struct world_t {
  using position_t = position_t<W, H>;

  using distance_t = size_t;
  distance_t distance(const position_t& from, const position_t& to) const
  {
    const auto dst = [](auto a, auto b) {
      return a > b ? a - b : b - a;
    };

    auto dx = std::min(dst(from.x, to.x), dst(from.x + W, to.x));
    auto dy = std::min(dst(from.y, to.y), dst(from.y + H, to.y));
    return dx * dx + dy * dy;
  }

  std::array<position_t, 4> neighborhs(const position_t& pos) const
  {
    return {pos.move(EDirection::North), pos.move(EDirection::East), pos.move(EDirection::South), pos.move(EDirection::West)};
  }

  std::vector<position_t> findPath(position_t start, position_t goal) const
  {
    using set_t = std::unordered_set<position_t, typename position_t::Hash>;
    using map_t = std::unordered_map<position_t, distance_t, typename position_t::Hash>;

    // The set of nodes already evaluated
    set_t closedSet{};

    // The set of currently discovered nodes that are not evaluated yet.
    // Initially, only the start node is known.
    set_t openSet;
    openSet.insert(start);

    // For each node, which node it can most efficiently be reached from.
    // If a node can be reached from many nodes, cameFrom will eventually contain the
    // most efficient previous step.
    std::unordered_map<position_t, position_t, typename position_t::Hash> cameFrom{};

    const auto Inf = W * W + H * H + 1;

    const auto getScore = [&](map_t& s, const position_t& pos) {
      return s.count(pos) ? s[pos] : Inf;
    };

    // For each node, the cost of getting from the start node to that node.
    map_t gScore{};

    // The cost of going from start to start is zero.
    gScore[start] = 0;

    // For each node, the total cost of getting from the start node to the goal
    // by passing by that node. That value is partly known, partly heuristic.
    map_t fScore{};

    // For the first node, that value is completely heuristic.
    fScore[start] = distance(start, goal);

    const auto findCurrent = [&]() -> position_t {
      auto score = Inf;
      const position_t* pos = nullptr;
      for (const auto& node : openSet) {
        auto f = getScore(fScore, node);
        if (f < score) {
          score = f;
          pos = &node;
        }
      }
      assert(pos != nullptr);
      return position_t{*pos};
    };

    const auto reconstruct_path = [&](const position_t& pos) {
      auto p = pos;
      std::vector<position_t> total_path;
      total_path.push_back(p);
      while (cameFrom.count(p)) {
        p = cameFrom[p];
        total_path.push_back(p);
      }
      return total_path;
    };
    while (!openSet.empty()) {
      auto current = findCurrent();
      if (current == goal) {
        return reconstruct_path(current);
      }
      openSet.erase(current);
      closedSet.insert(current);

      for (const auto& neighbor : neighborhs(current)) {
        if (closedSet.count(neighbor)) {
          continue;
        }

        // The distance from start to a neighbor
        auto tentative_gScore = getScore(gScore, current) + distance(current, neighbor);

        if (!openSet.count(neighbor)) // Discover a new node
          openSet.insert(neighbor);
        else if (tentative_gScore >= getScore(gScore, neighbor))
          continue; // This is not a better path.

        // This path is the best until now. Record it!
        cameFrom[neighbor] = current;
        gScore[neighbor] = tentative_gScore;
        fScore[neighbor] = getScore(gScore, neighbor) + distance(neighbor, goal);
      }
    }
    return {};
  }

private:
};

int main()
{
  std::cout << KVersion << '\n';
  std::cout << "Start game loop" << std::endl;

  const auto world = world_t<5, 5>{};
  using pos_t = decltype(world)::position_t;

  assert(2 == world.distance(pos_t{0, 0}, pos_t{1, 1}));
  assert(2 == world.distance(pos_t{0, 0}, pos_t{4, 4}));
  assert(1 == world.distance(pos_t{0, 0}, pos_t{4, 0}));

  for (const auto& p : world.findPath(pos_t{0, 0}, pos_t{4, 4})) {
    std::cout << p;
  }
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
