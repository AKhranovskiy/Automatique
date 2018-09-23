#include "command.h"
#include "cxhash.hpp"
#include "version.h"
#include "world.h"

#include <iostream>

template <coord_t W, coord_t H>
std::ostream& operator<<(std::ostream& os, const position_t<W, H>& pos)
{
  return (os << '(' << pos.x << ',' << pos.y << ')');
}

enum class ERobotState {
  Idle,
  Moving,
};

struct command_move_t {
  World::path_t path;
};

using robot_id = std::size_t;

class robot_t {
  const robot_id _id;
  World::position_t _pos;

  ERobotState _state;
  World::path_t _path;

  command_t _command;

public:
  explicit robot_t(robot_id id, World::position_t pos) noexcept
      : _id{id}
      , _pos{pos}
      , _state{ERobotState::Idle}
  {
  }

  robot_t(const robot_t&) = delete;
  robot_t(robot_t&&) = delete;
  robot_t& operator=(const robot_t&) = delete;
  robot_t& operator=(robot_t&&) = delete;

  robot_id id() const noexcept { return _id; }
  ERobotState state() const noexcept { return _state; }
  World::position_t position() const noexcept { return _pos; }

  result_t command(command_move_t o) noexcept
  {
    std::cout << "Robot#1 has received command MOVE-TO: ";
    if (_state != ERobotState::Idle) {
      std::cout << "rejected, is moving\n";
      return make_result(false);
    }
    if (o.path.empty()) {
      _state = ERobotState::Idle;

      std::cout << "done, empty path\n";
      return make_result(true);
    }
    if (_pos != o.path.back()) {
      std::cout << "rejected, wrong start position\n";
      return make_result(false);
    }

    o.path.pop_back();
    if (o.path.empty()) {
      _state = ERobotState::Idle;
      std::cout << "done, arrived\n";
      return make_result(true);
    }
    _path = std::move(o.path);
    _state = ERobotState::Moving;

    std::cout << "accepted, is moving\n";
    _command = command_t{};
    return _command.get_future();
  }

  bool tick() noexcept
  {
    switch (_state) {
    case ERobotState::Idle:
      return true;
    case ERobotState::Moving:
      std::cout << "Robot#" << _id << " is moving from " << _pos << " to " << _path.back() << '\n';
      _pos = _path.back();
      _path.pop_back();
      if (_path.empty()) {
        _state = ERobotState::Idle;

        std::cout << "Robot#" << _id << " has arrived\n";
        _command.set_value(true);
      }

      return true;
    }
  }
};

class robot_dispatcher {
  const World::position_t _start_pos;
  robot_id _next_id{1u};
  std::vector<std::unique_ptr<robot_t>> _robots;

public:
  robot_dispatcher(World::position_t pos)
      : _start_pos{std::move(pos)} {};

  robot_dispatcher(const robot_dispatcher&) = delete;
  robot_dispatcher(robot_dispatcher&&) = delete;
  robot_dispatcher& operator=(const robot_dispatcher&) = delete;
  robot_dispatcher& operator=(robot_dispatcher&&) = delete;

  bool create_robot() noexcept
  {
    _robots.emplace_back(std::make_unique<robot_t>(_next_id++, _start_pos));
    std::cout << "RobotDispatcher has created robot#" << _robots.back()->id() << '\n';
    return true;
  }

  result_t send_robot_to(const World::position_t& pos) noexcept
  {
    if (_robots.empty()) {
      std::cout << "RobotDispatcher has no robots to command\n";
      return make_result(false);
    }

    auto closestIdleRobot = _robots.cend();
    distance_t min_d = std::numeric_limits<distance_t>::max();

    auto it = _robots.cbegin();
    const auto is_idle = [](auto& robot) { return robot->state() == ERobotState::Idle; };

    while ((it = std::find_if(it, _robots.cend(), is_idle)) != _robots.cend()) {
      auto d = distance((**it).position(), pos);
      if (d < min_d) {
        min_d = d;
        closestIdleRobot = it;
      }
      ++it;
    }

    if (closestIdleRobot != _robots.cend()) {
      auto& r = **closestIdleRobot;
      if (min_d == 0) {
        std::cout << "RobotDispatcher has found an idle robot#" << r.id() << " at " << pos << '\n';
        return make_result(true);
      }
      std::cout << "RobotDispatcher has sent robot#" << r.id() << " to " << pos << '\n';
      auto path = findPath(r.position(), pos);
      return r.command(command_move_t{std::move(path)});
    }
    std::cout << "RobotDispatcher has no idle robots to send to " << pos << '\n';
    return make_result(false);
  }

  void tick() noexcept
  {
    for (const auto& r : _robots) {
      r->tick();
    }
  }
};

int main()
{
  std::cout << KVersion << '\n';
  std::cout << "Start game loop" << std::endl;

  assert(2 == distance(World::position_t{0, 0}, World::position_t{1, 1}));
  assert(2 == distance(World::position_t{0, 0}, World::position_t{4, 4}));
  assert(1 == distance(World::position_t{0, 0}, World::position_t{4, 0}));

  robot_dispatcher rd{World::position_t{0, 0}};
  rd.create_robot();
  rd.create_robot();
  auto r1 = rd.send_robot_to(World::position_t{3, 3});
  auto r2 = rd.send_robot_to(World::position_t{1, 1});

  while (!is_command_done(r1) || !is_command_done(r2)) {
    rd.tick();
  }

  std::cout << "1: " << r1.get() << ", 2: " << r2.get() << '\n';
  std::cout << "Finish game loop\n"
            << std::endl;
  return 0;
}
