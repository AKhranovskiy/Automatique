#pragma once

// class robot_dispatcher {
//   const World::position_t _start_pos;
//   robot_id _next_id{1u};
//
//   using profile_variant_t = std::variant<profile_none, profile_scout>;
//   struct profiled_robot {
//
//     std::unique_ptr<robot_t> robot;
//     profile_variant_t profile;
//
//     profiled_robot(std::unique_ptr<robot_t>&& r, profile_variant_t&& p) noexcept
//         : robot{std::move(r)}, profile{std::move(p)} {}
//   };
//
//   std::unordered_map<robot_id, profiled_robot> _robots;
//
// public:
//   explicit robot_dispatcher(World::position_t pos) : _start_pos{pos} {};
//
//   robot_dispatcher(const robot_dispatcher&) = delete;
//   robot_dispatcher(robot_dispatcher&&) = delete;
//   robot_dispatcher& operator=(const robot_dispatcher&) = delete;
//   robot_dispatcher& operator=(robot_dispatcher&&) = delete;
//
//   bool create_robot() noexcept {
//     const auto id = _next_id++;
//
//     auto robot = std::make_unique<robot_t>(id, _start_pos);
//     auto profile = profile_variant_t{std::in_place_type<profile_none>, *robot};
//
//     const auto [it, ok] = _robots.try_emplace(id, std::move(robot), std::move(profile));
//     if (ok) {
//       std::cout << "RobotDispatcher has created robot#" << id << '\n';
//     }
//     return ok;
//   }
//
//   result_t discover() noexcept {
//     // Find idle robot
//
//     if (_robots.empty()) {
//       std::cout << "RobotDispatcher has no robots to command\n";
//       return make_result(false);
//     }
//
//     const auto is_idle_scout = [](const auto& kv) {
//       const auto& [id, r] = kv;
//       return std::holds_alternative<profile_scout>(r.profile) &&
//              r.robot->state() == ERobotState::Idle;
//     };
//
//     auto idleScout = std::find_if(_robots.begin(), _robots.end(), is_idle_scout);
//
//     if (idleScout == _robots.end()) {
//       // Try to make scout
//       const auto is_idle = [](const auto& kv) {
//         const auto& [id, r] = kv;
//         return r.robot->state() == ERobotState::Idle;
//       };
//       auto idleRobot = std::find_if(_robots.begin(), _robots.end(), is_idle);
//       if (idleRobot == _robots.end()) {
//         return make_result(false);
//       }
//       auto& [id, rp] = *idleRobot;
//       rp.profile.emplace<profile_scout>(*rp.robot);
//       idleScout = idleRobot;
//     }
//
//     if (idleScout != _robots.end()) {
//       auto& [id, rp] = *idleScout;
//       auto* scout = std::get_if<profile_scout>(&rp.profile);
//       assert(scout != nullptr);
//       return scout->command(command_discover_t{});
//     }
//
//     return make_result(false);
//   }
//
//   result_t send_robot_to(const World::position_t& pos) noexcept {
//     if (_robots.empty()) {
//       std::cout << "RobotDispatcher has no robots to command\n";
//       return make_result(false);
//     }
//
//     auto closestIdleRobotId = robot_id{0u};
//     distance_t min_d = std::numeric_limits<distance_t>::max();
//
//     auto it = _robots.cbegin();
//     const auto is_idle = [](const auto& kv) {
//       const auto& [id, r] = kv;
//       return r.robot->state() == ERobotState::Idle;
//     };
//
//     while ((it = std::find_if(it, _robots.cend(), is_idle)) != _robots.cend()) {
//       const auto& [id, r] = *it;
//       const auto& robot = *r.robot;
//
//       auto d = distance(robot.position(), pos);
//       if (d < min_d) {
//         min_d = d;
//         closestIdleRobotId = id;
//       }
//       ++it;
//     }
//
//     if (closestIdleRobotId != 0) {
//       assert(_robots.count(closestIdleRobotId));
//       auto& r = *_robots.at(closestIdleRobotId).robot;
//       if (min_d == 0) {
//         std::cout << "RobotDispatcher has found an idle robot#" << r.id() << " at " << pos <<
//         '\n'; return make_result(true);
//       }
//       std::cout << "RobotDispatcher has sent robot#" << r.id() << " to " << pos << '\n';
//       auto path = findPath(r.position(), pos);
//       return r.command(command_move_t{std::move(path)});
//     }
//     std::cout << "RobotDispatcher has no idle robots to send to " << pos << '\n';
//     return make_result(false);
//   }
//
//   result_t stop() {
//     for (auto& [id, r] : _robots) {
//       r.robot->command(command_stop_t{});
//     }
//     return make_result(true);
//   }
//   void tick() noexcept {
//     for (auto& [id, r] : _robots) {
//       std::visit([](auto&& profile) { profile.tick(); }, r.profile);
//     }
//   }
// };
