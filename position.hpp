#pragma once

#include "cxhash.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

enum class EDirection { North, East, South, West };

using coord_t = std::size_t;

template <coord_t W, coord_t H> struct position_t {
  static_assert(W > 2, "Width is too small");
  static_assert(H > 2, "Height is too small");

  coord_t x{0};
  coord_t y{0};

  constexpr position_t() noexcept = default;
  constexpr position_t(const position_t& pos) noexcept = default;
  constexpr position_t(position_t&& pos) noexcept = default;

  constexpr position_t(coord_t x, coord_t y) noexcept : x{x % W}, y{y % H} {}

  constexpr position_t& operator=(const position_t& pos) noexcept = default;
  constexpr position_t& operator=(position_t&& pos) noexcept = default;
};

// TODO Spaceship operator?
template <coord_t W, coord_t H, typename pos_t = position_t<W, H>>
constexpr bool operator==(const position_t<W, H>& lhs, const pos_t& rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}
template <coord_t W, coord_t H, typename pos_t = position_t<W, H>>
constexpr bool operator!=(const position_t<W, H>& lhs, const pos_t& rhs) noexcept {
  return !(lhs == rhs);
}
template <coord_t W, coord_t H, typename pos_t = position_t<W, H>>
constexpr bool operator<(const position_t<W, H>& lhs, const pos_t& rhs) noexcept {
  return lhs.x < rhs.x && lhs.y < rhs.y;
}
template <coord_t W, coord_t H, typename pos_t = position_t<W, H>>
constexpr bool operator>(const position_t<W, H>& lhs, const pos_t& rhs) noexcept {
  return lhs.x > rhs.x && lhs.y > rhs.y;
}

template <coord_t W, coord_t H, typename pos_t = position_t<W, H>>
constexpr pos_t move(const position_t<W, H>& pos, EDirection dir) noexcept {
  switch (dir) {
  case EDirection::North:
    return {pos.x, pos.y > 0 ? pos.y - 1 : H - 1};
  case EDirection::East:
    return {pos.x + 1, pos.y};
  case EDirection::South:
    return {pos.x, pos.y + 1};
  case EDirection::West:
    return {pos.x > 0 ? pos.x - 1 : W - 1, pos.y};
  }
}

template <coord_t W, coord_t H> struct position_hash {
  std::size_t operator()(const position_t<W, H>& p) const noexcept { return cx::FNVhash(p.x, p.y); }
};

using distance_t = decltype(std::declval<coord_t>() * std::declval<coord_t>());

template <coord_t W, coord_t H, typename pos_t = position_t<W, H>>
constexpr distance_t distance(const position_t<W, H>& from, const pos_t& to) noexcept {
  const auto dst = [](auto a, auto b) { return a > b ? a - b : b - a; };

  auto dx = std::min(dst(from.x, to.x), dst(from.x + W, to.x));
  auto dy = std::min(dst(from.y, to.y), dst(from.y + H, to.y));
  return dx * dx + dy * dy;
}

template <coord_t W, coord_t H>
constexpr std::array<position_t<W, H>, 4> neighborhs(const position_t<W, H>& pos) noexcept {
  return {move(pos, EDirection::North), move(pos, EDirection::East), move(pos, EDirection::South),
          move(pos, EDirection::West)};
}

template <coord_t W, coord_t H> using path_t = std::vector<position_t<W, H>>;

template <coord_t W, coord_t H, typename pos_t = position_t<W, H>>
path_t<W, H> findPath(const position_t<W, H>& start, const pos_t& goal) noexcept {
  using position_hash_t = position_hash<W, H>;

  using set_t = std::unordered_set<pos_t, position_hash_t>;
  using score_map_t = std::unordered_map<pos_t, distance_t, position_hash_t>;
  using came_from_map_t = std::unordered_map<pos_t, pos_t, position_hash_t>;

  const auto Inf = W * W + H * H + 1;

  // For non-existing key, map creates default value.
  // But it should be Inf by default.
  // I return Inf instead of default value to not fill entire map with all possible keys.
  const auto getScore = [&Inf](score_map_t& score_map, const pos_t& pos) -> distance_t {
    return score_map.count(pos) ? score_map[pos] : Inf;
  };

  // The set of nodes already evaluated
  set_t closedSet{};

  // The set of currently discovered nodes that are not evaluated yet.
  // Initially, only the start node is known.
  set_t openSet;
  openSet.insert(start);

  // For each node, which node it can most efficiently be reached from.
  // If a node can be reached from many nodes, cameFrom will eventually contain the
  // most efficient previous step.
  came_from_map_t cameFrom{};

  // For each node, the cost of getting from the start node to that node.
  score_map_t gScore{};

  // The cost of going from start to start is zero.
  gScore[start] = 0;

  // For each node, the total cost of getting from the start node to the goal
  // by passing by that node. That value is partly known, partly heuristic.
  score_map_t fScore{};

  // For the first node, that value is completely heuristic.
  fScore[start] = distance(start, goal);

  // Find node in openSet with minimal fScore
  const auto findCurrent = [&]() -> pos_t {
    auto score = Inf;
    const pos_t* pos = nullptr;
    for (const auto& node : openSet) {
      auto f = getScore(fScore, node);
      if (f < score) {
        score = f;
        pos = &node;
      }
    }
    assert(pos != nullptr);
    return pos_t{*pos};
  };

  const auto reconstruct_path = [&](const pos_t& pos) {
    auto p = pos;
    path_t<W, H> total_path;
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

      if (!openSet.count(neighbor)) { // Discover a new node
        openSet.insert(neighbor);
      } else if (tentative_gScore >= getScore(gScore, neighbor)) {
        continue; // This is not a better path.
      }

      // This path is the best until now. Record it!
      cameFrom[neighbor] = current;
      gScore[neighbor] = tentative_gScore;
      fScore[neighbor] = getScore(gScore, neighbor) + distance(neighbor, goal);
    }
  }
  return {};
}
