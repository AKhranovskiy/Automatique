#include "commands.h"

#include "future_utils.hpp"
#include "robot.h"

#include <type_traits>

template <class Required, class Given> Required* safe_cast(Given& given) noexcept {
  if constexpr (std::is_base_of_v<Required, Given>)
    return static_cast<Required*>(&given);
  else
    return dynamic_cast<Required*>(&given);
}

command_move_t::result_t command_move_t::assign(robot_t& robot) noexcept {
  const auto make_result = [](EResult r) {
    return std::make_tuple(false, operation_t{}, future_utils::make_ready_future(std::move(r)));
  };

  auto r = safe_cast<robot_t>(robot);
  if (r == nullptr) {
    return make_result(EResult::NotSupported);
  }
  if (!r->idle()) {
    return make_result(EResult::Busy);
  }
  if (_path.empty()) {
    return make_result(EResult::Ok);
  }
  if (r->position() != _path.back()) {
    return make_result(EResult::WrongPath);
  }

  struct Action {
    World::path_t path;
    std::promise<EResult> promise;
    robot_t& robot;

    Action(World::path_t&& path, robot_t& r) : path{std::move(path)}, robot{r} {}

    bool operator()() noexcept {
      assert(path.empty() || robot.position() == path.back());

      if (!path.empty()) {
        path.pop_back();
      }
      if (path.empty()) {
        promise.set_value(EResult::Ok);
      } else if (!robot.move(path.back())) {
        promise.set_value(EResult::Failed);
      }
      return path.empty();
    }
  };

  Action action{std::move(_path), *r};
  auto f = action.promise.get_future();
  return std::make_tuple(true, operation_t{std::move(action)}, std::move(f));
}
