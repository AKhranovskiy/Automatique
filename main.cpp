#include "control_module.hpp"
#include "future_utils.hpp"
#include "pretty_print.hpp"
#include "traits.h"
#include "unit.h"
#include "version.h"
#include "world.h"
#include <iostream>

using future_utils::get_result;
using future_utils::is_future_ready;

int main() {
  std::cout << KVersion << '\n';
  std::cout << "Start game loop" << std::endl;

  unit_t u{1u, {1, 1}};
  std::cout << u << " " << u.position << '\n';

  auto c = control_module_t<unit_t, trait_move_t, trait_ping_t>{u};
  auto f = c.start().move(findPath(u.position, {4, 3})).ping().finish();
  auto idle = c.idle();
  std::cout << "Idle: " << is_future_ready(idle) << '\n';
  while (!is_future_ready(f)) c();
  std::cout << "Idle: " << is_future_ready(idle) << '\n';
  auto& u2 = f.get();
  std::cout << u2 << " " << u2.position << '\n';
  std::cout << "Idle: " << is_future_ready(c.idle()) << '\n';

  std::cout << "Finish game loop\n" << std::endl;
  return 0;
}
