#include "modules/discovery.h"
#include "modules/mining.h"
#include "utils/future_combine.hpp"

template <class Operation, class Result> void run(Operation& operation, Result&& result) noexcept {
  while (!future_utils::is_future_ready(result)) operation();
}

int main() {
  World::Chronicles().Prolog();

  unit_basic_t u{1u, {1, 1}};
  unit_basic_t u2{2u, {3, 3}};

  auto discovery = module_discovery_t{1u, {2, 2}};
  auto discovered = discovery.discover(4);
  discovered = discovery.assign({u, u2});
  run(discovery, discovered);

  World::Chronicles().Log(World{});

  const auto tiles_coal = World::get_areas({2, 2}, 4, ETileContent::Coal);
  const auto tiles_ore = World::get_areas({2, 2}, 4, ETileContent::Ore);
  const auto tiles_water = World::get_areas({2, 2}, 4, ETileContent::Water);

  World::Chronicles().Log("Coal:") << tiles_coal << "\n";
  World::Chronicles().Log("Ore:") << tiles_ore << "\n";
  World::Chronicles().Log("Water:") << tiles_water << "\n";

  unit_carrier_t m{3u, {2, 2}};
  unit_carrier_t m2{4u, {2, 2}};
  unit_carrier_t m3{5u, {2, 2}};

  auto mining = module_mining_t{2u, {2, 2}};
  mining.assign({m, m2, m3});

  for (auto i = 0u; i < 3u; ++i) {
    auto res = mining.mine(tiles_coal.front()) && mining.mine(tiles_ore.front()) &&
               mining.mine(tiles_water.front());
    while (!res) mining();

    World::Chronicles().Log("Warehouse")
        << "has:\n"
        << "\tCoal: " << mining.get_warehouse().get_resource_amount(EResourceType::Coal) << "\n"
        << "\tOre: " << mining.get_warehouse().get_resource_amount(EResourceType::Ore) << "\n"
        << "\tWater: " << mining.get_warehouse().get_resource_amount(EResourceType::Water) << "\n";
  }

  World::Chronicles().Epilog();
  return 0;
}
