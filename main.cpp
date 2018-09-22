#include "cxhash.hpp"
#include "position.hpp"
#include "version.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

template <coord_t W, coord_t H>
std::ostream& operator<<(std::ostream& os, const position_t<W, H>& pos)
{
  return (os << '(' << pos.x << ',' << pos.y << ')');
}

enum class ERobotState {
  Idle,
  Moving,
};

struct World {
  static constexpr const coord_t WIDTH{5u};
  static constexpr const coord_t HEIGHT{5u};

  static constexpr const std::chrono::milliseconds TICK{200};

  using position_t = position_t<WIDTH, HEIGHT>;
  using path_t = path_t<WIDTH, HEIGHT>;

  static void sleep() noexcept
  {
    std::this_thread::sleep_for(World::TICK);
  }
};

struct order_move_t {
  World::path_t path;
};

using robot_id = std::size_t;

class robot_t {
  const robot_id _id;
  World::position_t _pos;

  ERobotState _state;
  World::path_t _path;

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

  bool order(order_move_t o) noexcept
  {
    if (o.path.empty()) {
      _state = ERobotState::Idle;
      return true;
    }
    if (_pos != o.path.back()) {
      return false;
    }
    o.path.pop_back();
    if (o.path.empty()) {
      _state = ERobotState::Idle;
      return true;
    }
    _path = std::move(o.path);
    _state = ERobotState::Moving;
    return true;
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

  bool send_robot_to(const World::position_t& pos) noexcept
  {
    auto idleRobotIt = std::find_if(_robots.cbegin(), _robots.cend(), [](auto& robot) {
      return robot->state() == ERobotState::Idle;
    });
    if (idleRobotIt != _robots.cend()) {
      auto& r = **idleRobotIt;
      std::cout << "RobotDispatcher has sent robot#" << r.id() << " to " << pos << '\n';
      auto path = findPath(r.position(), pos);
      return r.order(order_move_t{std::move(path)});
    }
    std::cout << "RobotDispatcher has no idle robots to send to " << pos << '\n';
    return false;
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
  rd.send_robot_to(World::position_t{3, 3});

  bool quitRequested = false;
  while (!quitRequested) {
    // while (std::string::traits_type::not_eof(std::cin.peek())) {
    //   std::cout << (uint32_t)std::cin.peek();
    //   if (std::cin.get() == 'q') {
    //     std::cout << "Quit\n";
    //     quitRequested = true;
    //     break;
    //   }
    // }

    rd.tick();

    World::sleep();

    rd.send_robot_to(World::position_t{1, 1});
  }

  std::cout << "Finish game loop\n"
            << std::endl;
  return 0;
}
