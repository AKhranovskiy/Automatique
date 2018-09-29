#pragma once

#include "operation.hpp"
#include "world.h"
#include <future>
#include <tuple>

class robot_t;
struct command_move_t {
  enum class EResult { Ok, NotSupported, Busy, WrongPath, Failed };

  explicit command_move_t(World::path_t path) : _path{std::move(path)} {}

  using result_t = std::tuple<bool, operation_t, std::future<EResult>>;
  result_t assign(robot_t& robot) noexcept;

private:
  World::path_t _path;
};
