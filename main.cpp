#include "control_module.hpp"
#include "discovery_module.h"
#include "pretty_print.hpp"
#include "traits.h"
#include "unit.h"
#include "version.h"
#include "world.h"
#include <iostream>

// template <class Trait, class ControlCenter>
// std::optional<std::reference_wrapper<ControlCenter>> get_robot(ControlCenter& control) noexcept {
//   if constexpr (std::is_same_v<Trait, typename ControlCenter::object_t>)
//     return std::make_optional(std::ref(control));
//   return std::nullopt;
// }

int main() {
  std::cout << KVersion << '\n';
  std::cout << "Start game loop" << std::endl;

  unit_t u{1u, {1, 1}};
  auto scout = control_module_t<unit_t, trait_move_t, trait_discover_t>{u};
  std::cout << u << " " << u.position << '\n';

  unit_t u2{2u, {3, 3}};
  auto scout2 = control_module_t<unit_t, trait_move_t, trait_discover_t>{u2};
  std::cout << u2 << " " << u2.position << '\n';

  auto discovery = discovery_module_t{1u, {2, 2}};

  auto discovered = discovery.discover(1);
  run(discovery, discovered);

  discovered = discovery.assign(scout);
  discovery();
  discovery();
  discovered = discovery.assign(scout2);
  run(discovery, discovered);
  std::cout << discovery << " has finished.\n";

  std::cout << "Finish game loop\n" << std::endl;
  return 0;
}
