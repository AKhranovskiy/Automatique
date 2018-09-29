#include "commands.h"
#include "future_utils.hpp"
#include "pretty_print.hpp"
#include "robot.h"
#include "version.h"
#include "world.h"
#include <iostream>

using future_utils::get_result;
using future_utils::is_future_ready;

int main() {
  std::cout << KVersion << '\n';
  std::cout << "Start game loop" << std::endl;

  robot_t r{1, {0, 0}};
  std::cout << "id\t" << r.id() << '\n';
  std::cout << "idle\t" << r.idle() << '\n';
  std::cout << "pos\t" << r.position() << '\n';

  auto result = r.order(command_move_t{findPath(r.position(), {3, 2})});
  std::cout << "idle\t" << r.idle() << '\n';

  while (!is_future_ready(result)) {
    r.tick();
  };

  std::cout << "result\t" << get_result(result) << '\n';
  std::cout << "idle\t" << r.idle() << '\n';
  std::cout << "pos\t" << r.position() << '\n';

  std::cout << "Finish game loop\n" << std::endl;
  return 0;
}
