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

template <coord_t W, coord_t H>
struct order_move_t {
  path_t<W, H> path;
};

using robot_id = std::size_t;
template <coord_t W, coord_t H>
class robot_t {
  const robot_id _id;
  position_t<W, H> _pos;

  ERobotState _state;
  path_t<W, H> _path;

public:
  robot_t(robot_id id, position_t<W, H> pos) noexcept
      : _id{id}
      , _pos{pos}
      , _state{ERobotState::Idle}
  {
  }

  robot_t(const robot_t&) = delete;
  robot_t(robot_t&&) = delete;
  robot_t& operator=(const robot_t&) = delete;
  robot_t& operator=(robot_t&&) = delete;

  ERobotState state() const noexcept { return _state; }
  position_t<W, H> position() const noexcept { return _pos; }

  bool order(order_move_t<W, H> o) noexcept
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

struct World {
  static constexpr const coord_t WIDTH{5u};
  static constexpr const coord_t HEIGHT{5u};

  static constexpr const std::chrono::milliseconds TICK{200};

  using position_t = position_t<WIDTH, HEIGHT>;
  using path_t = path_t<WIDTH, HEIGHT>;
  using robot_t = robot_t<WIDTH, HEIGHT>;
  using order_move_t = order_move_t<WIDTH, HEIGHT>;
};

int main()
{
  std::cout << KVersion << '\n';
  std::cout << "Start game loop" << std::endl;

  assert(2 == distance(World::position_t{0, 0}, World::position_t{1, 1}));
  assert(2 == distance(World::position_t{0, 0}, World::position_t{4, 4}));
  assert(1 == distance(World::position_t{0, 0}, World::position_t{4, 0}));

  World::robot_t robot{1, World::position_t{0, 0}};
  robot.order(World::order_move_t{findPath(robot.position(), World::position_t{3, 3})});

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

    robot.tick();

    std::this_thread::sleep_for(World::TICK);
  }

  std::cout << "Finish game loop\n"
            << std::endl;
  return 0;
}
