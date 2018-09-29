#pragma once

#include "operation.hpp"
#include "world.h"
#include <queue>

using robot_id = std::size_t;

class robot_t {
  const robot_id _id;
  World::position_t _pos;

  std::queue<operation_t> _operations;

public:
  explicit robot_t(robot_id id, World::position_t pos) noexcept : _id{id}, _pos{pos} {}

  robot_t(const robot_t&) = delete;
  robot_t& operator=(const robot_t&) = delete;

  robot_t(robot_t&&) = default;
  robot_t& operator=(robot_t&&) = default;

  constexpr inline robot_id id() const noexcept { return _id; }
  constexpr inline bool idle() const noexcept { return _operations.empty(); }
  constexpr inline World::position_t position() const noexcept { return _pos; }

  template <class Command> auto order(Command cmd) noexcept {
    auto [assigned, operation, result] = cmd.assign(*this);
    if (assigned) _operations.push(std::move(operation));
    return std::move(result);
  }

  inline bool tick() noexcept {
    if (!idle() && _operations.front()()) _operations.pop();
    return idle();
  }

  bool move(World::position_t pos) noexcept;
};
