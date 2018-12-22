#include "modules/core.hpp"
#include "world.h"

template <class Operation, class Result> void run(Operation& operation, Result&& result) noexcept {
  while (!future_utils::is_future_ready(result)) operation();
}
template <class Operation> void run(Operation& operation) noexcept {
  while (!operation()) {
  };
}

int main() {
  World::Chronicles().Prolog();

  module_core_t core{{2, 2},
                     config::start::unit<unit_carrier_t, 2>{},
                     config::start::module<module_discovery_t>{},
                     config::start::module<module_mining_t>{}};
  if (auto discovery = core.module<module_discovery_t>(); discovery) {
    unpack(discovery).discover(4);
  }
  if (auto mining = core.module<module_mining_t>(); mining) {
    unpack(mining).mine(ETileContent::Ore, 30, module_mining_t::immediate);
    unpack(mining).mine(ETileContent::Water, 100, module_mining_t::deferred);
    unpack(mining).mine(ETileContent::Coal, 50, module_mining_t::deferred);
  }

  run(core);

  if (auto mining = core.module<module_mining_t>(); mining) {
    World::Chronicles().Log("Warehouse")
        << "has:\n"
        << "\tCoal: " << unpack(mining).get_warehouse().get_resource_amount(EResourceType::Coal)
        << "\n"
        << "\tOre: " << unpack(mining).get_warehouse().get_resource_amount(EResourceType::Ore)
        << "\n"
        << "\tWater: " << unpack(mining).get_warehouse().get_resource_amount(EResourceType::Water)
        << "\n";
  }

  World::Chronicles().Epilog();
  return 0;
}
