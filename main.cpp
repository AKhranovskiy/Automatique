#include "command.h"
#include "cxhash.hpp"
#include "version.h"
#include "world.h"

#include <iostream>
#include <thread>
#include <variant>

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
struct command_stop_t {
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
    std::cout << "Robot#" << _id << " has received command MOVE-TO: ";
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

  result_t command(command_stop_t) noexcept
  {
    std::cout << "Robot#" << _id << " has received command STOP: ";
    switch (_state) {
    case ERobotState::Idle:
      std::cout << "idling, done\n";
      return make_result(true);
    case ERobotState::Moving:
      std::cout << "moving, stop, done\n";
      _state = ERobotState::Idle;
      _path.clear();
      _command.set_value(false);
      _command = command_t{};
      return make_result(true);
    }
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

class profile_none {
  robot_t& _robot;

public:
  explicit profile_none(robot_t& robot) noexcept
      : _robot(robot)
  {
  }

  profile_none(const profile_none&) = delete;
  profile_none(profile_none&&) = default;
  profile_none& operator=(const profile_none&) = delete;
  profile_none& operator=(profile_none&&) = default;

  bool tick() noexcept
  {
    return _robot.tick();
  }
};

struct command_discover_t {
};

class profile_scout {
  robot_t& _robot;

public:
  explicit profile_scout(robot_t& robot) noexcept
      : _robot(robot)
  {
  }

  profile_scout(const profile_scout&) = delete;
  profile_scout(profile_scout&&) = default;
  profile_scout& operator=(const profile_scout&) = delete;
  profile_scout& operator=(profile_scout&&) = default;

  result_t command(command_discover_t) noexcept
  {
    std::cout << "Scout#" << _robot.id() << " received command DISCOVER: ";
    std::cout << "\n\tsearching for UNKNOWN area around " << _robot.position();

    auto pos = _robot.position();
    auto target = find_unknown(pos);
    if (pos == target) {
      std::cout << "\n\tfailed to find UNKNOWN area. Complete.\n";
      return make_result(false);
    }
    std::cout << "\n\tfound UNKNOWN area " << target;
    auto path = findPath(pos, target);

    auto moving = _robot.command(command_move_t{std::move(path)});
    //return combine_result(moving, command(command_analyze_t{}));
    return moving;
  }

  bool tick() noexcept
  {
    return _robot.tick();
  }

  World::position_t find_unknown(const World::position_t& pos) const noexcept
  {
    const auto mv = [](const auto& pos, EDirection dir, size_t steps) {
      auto p = pos;
      for (auto i = 0u; i < steps; ++i)
        p = move(p, dir);
      return p;
    };

    auto p = mv(pos, EDirection::North, 1);
    std::cout << "\n\t\tcheck " << p;
    size_t s = 1;
    size_t total = 0;
    std::underlying_type_t<EDirection> dir = 0;
    while (!World::Tiles.count(p) || World::Tiles.at(p) != ETileContent::Unknown) {
      s += dir % 2 == 0;
      std::cout << ": skip\n";
      p = mv(p, static_cast<EDirection>(dir), s);
      std::cout << "\n\t\tcheck " << p;
      dir = (dir + 1) % 4;
      total++;
      if (total / World::WIDTH == World::HEIGHT) {
        return pos;
      }
    }
    std::cout << ": done\n";
    return pos;
  }
};

class robot_dispatcher {
  const World::position_t _start_pos;
  robot_id _next_id{1u};

  using profile_variant_t = std::variant<profile_none, profile_scout>;
  struct profiled_robot {

    std::unique_ptr<robot_t> robot;
    profile_variant_t profile;

    profiled_robot(std::unique_ptr<robot_t>&& r, profile_variant_t&& p) noexcept
        : robot{std::move(r)}
        , profile{std::move(p)}
    {
    }
  };

  std::unordered_map<robot_id, profiled_robot> _robots;

public:
  robot_dispatcher(World::position_t pos)
      : _start_pos{std::move(pos)} {};

  robot_dispatcher(const robot_dispatcher&) = delete;
  robot_dispatcher(robot_dispatcher&&) = delete;
  robot_dispatcher& operator=(const robot_dispatcher&) = delete;
  robot_dispatcher& operator=(robot_dispatcher&&) = delete;

  bool create_robot() noexcept
  {
    const auto id = _next_id++;

    auto robot = std::make_unique<robot_t>(id, _start_pos);
    auto profile = profile_variant_t{std::in_place_type<profile_none>, *robot};

    const auto [it, ok] = _robots.try_emplace(id, std::move(robot), std::move(profile));
    if (ok) {
      std::cout << "RobotDispatcher has created robot#" << id << '\n';
    }
    return ok;
  }

  result_t discover() noexcept
  {
    // Find idle robot

    if (_robots.empty()) {
      std::cout << "RobotDispatcher has no robots to command\n";
      return make_result(false);
    }

    const auto is_idle_scout = [](const auto& kv) {
      const auto& [id, r] = kv;
      return std::holds_alternative<profile_scout>(r.profile)
          && r.robot->state() == ERobotState::Idle;
    };

    auto idleScout = std::find_if(_robots.begin(), _robots.end(), is_idle_scout);

    if (idleScout == _robots.end()) {
      // Try to make scout
      const auto is_idle = [](const auto& kv) {
        const auto& [id, r] = kv;
        return r.robot->state() == ERobotState::Idle;
      };
      auto idleRobot = std::find_if(_robots.begin(), _robots.end(), is_idle);
      if (idleRobot == _robots.end()) {
        return make_result(false);
      }
      auto& [id, rp] = *idleRobot;
      rp.profile.emplace<profile_scout>(*rp.robot);
      idleScout = idleRobot;
    }

    if (idleScout != _robots.end()) {
      auto& [id, rp] = *idleScout;
      auto* scout = std::get_if<profile_scout>(&rp.profile);
      assert(scout != nullptr);
      return scout->command(command_discover_t{});
    }

    return make_result(false);
  }

  result_t send_robot_to(const World::position_t& pos) noexcept
  {
    if (_robots.empty()) {
      std::cout << "RobotDispatcher has no robots to command\n";
      return make_result(false);
    }

    auto closestIdleRobotId = robot_id{0u};
    distance_t min_d = std::numeric_limits<distance_t>::max();

    auto it = _robots.cbegin();
    const auto is_idle = [](const auto& kv) {
      const auto& [id, r] = kv;
      return r.robot->state() == ERobotState::Idle;
    };

    while ((it = std::find_if(it, _robots.cend(), is_idle)) != _robots.cend()) {
      const auto& [id, r] = *it;
      const auto& robot = *r.robot;

      auto d = distance(robot.position(), pos);
      if (d < min_d) {
        min_d = d;
        closestIdleRobotId = id;
      }
      ++it;
    }

    if (closestIdleRobotId != 0) {
      assert(_robots.count(closestIdleRobotId));
      auto& r = *_robots.at(closestIdleRobotId).robot;
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

  result_t stop()
  {
    for (auto& [id, r] : _robots) {
      r.robot->command(command_stop_t{});
    }
    return make_result(true);
  }
  void tick() noexcept
  {
    for (auto& [id, r] : _robots) {
      std::visit([](auto&& profile) { profile.tick(); }, r.profile);
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

  while (!is_command_done(r1) && !is_command_done(r2)) {
    rd.tick();
  }

  auto discovery = rd.discover();
  while (!is_command_done(discovery)) {
    rd.tick();
  }

  std::cout << "Discovery " << discovery.get() << '\n';

  std::cout << "Finish game loop\n"
            << std::endl;
  return 0;
}
