#include "modules/discovery.h"
#include "modules/mining.h"
#include "utils/future_combine.hpp"

template <class Operation, class Result> void run(Operation& operation, Result&& result) noexcept {
  while (!future_utils::is_future_ready(result)) operation();
}

auto create_scout(unit_basic_t& unit) {
  return control_module_t<unit_basic_t, trait_move_t, trait_discover_t>{unit};
}

auto create_miner(unit_carrier_t& miner) {
  return control_module_t<unit_carrier_t, trait_move_t, trait_mine_t>{miner};
}

int main() {
  World::Chronicles().Prolog();

  unit_basic_t u{1u, {1, 1}};
  unit_basic_t u2{2u, {3, 3}};

  auto scout = create_scout(u);
  auto scout2 = create_scout(u2);

  auto discovery = discovery_module_t{1u, {2, 2}};

  auto discovered = discovery.discover(4);
  discovered = discovery.assign({scout, scout2});
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
  auto miner = create_miner(m);
  auto miner2 = create_miner(m2);
  auto miner3 = create_miner(m3);

  auto mining = mining_module_t{2u, {2, 2}};
  mining.assign({miner, miner2, miner3});

  auto res = mining.mine(tiles_coal.front()) && mining.mine(tiles_ore.front()) &&
             mining.mine(tiles_water.front());
  while (!res) mining();

  World::Chronicles().Log("Warehouse")
      << "has:\n"
      << "\tCoal: " << mining.get_warehouse().get_resource_amount(EResourceType::Coal) << "\n"
      << "\tOre: " << mining.get_warehouse().get_resource_amount(EResourceType::Ore) << "\n"
      << "\tWater: " << mining.get_warehouse().get_resource_amount(EResourceType::Water) << "\n";

  World::Chronicles().Epilog();
  return 0;
}
