#include "discovery_module.h"

// template <class Trait, class ControlCenter>
// std::optional<std::reference_wrapper<ControlCenter>> get_robot(ControlCenter& control) noexcept {
//   if constexpr (std::is_same_v<Trait, typename ControlCenter::object_t>)
//     return std::make_optional(std::ref(control));
//   return std::nullopt;
// }

int main() {
  World::Chronicles().Prolog();

  unit_t u{1u, {1, 1}};
  auto scout = control_module_t<unit_t, trait_move_t, trait_discover_t>{u};
  World::Chronicles().Log(u) << " " << u.position << '\n';

  unit_t u2{2u, {3, 3}};
  auto scout2 = control_module_t<unit_t, trait_move_t, trait_discover_t>{u2};
  World::Chronicles().Log(u2) << " " << u2.position << '\n';

  auto discovery = discovery_module_t{1u, {2, 2}};

  auto discovered = discovery.discover(8);
  run(discovery, discovered);

  discovered = discovery.assign(scout);
  discovery();
  discovery();
  discovered = discovery.assign(scout2);
  run(discovery, discovered);
  World::Chronicles().Log(discovery) << " has finished.\n";

  World::Chronicles().Log(World{});

  World::Chronicles().Epilog();
  return 0;
}
