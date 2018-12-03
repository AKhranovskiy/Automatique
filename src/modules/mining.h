#pragma once

#include "modules/base_unit_control.h"
#include "modules/warehouse.h"
#include "primitives/operation.h"
#include "traits/mine.hpp"
#include "traits/move.hpp"
#include "world.h"
#include <list>
#include <set>

class module_mining_t : public base_unit_control_t<unit_carrier_t, trait_move_t, trait_mine_t> {
public:
  explicit module_mining_t(module_id_t id, World::position_t position) noexcept;

  result_t mine(World::position_t position) noexcept;

  result_t assign(unit_carrier_t& unit) noexcept;
  result_t assign(unit_ref_list_t&& units) noexcept;

  const module_warehouse_t& get_warehouse() const noexcept { return _warehouse; }

private:
  std::set<World::position_t> _income_queue;
  module_warehouse_t _warehouse;

  struct task_t {
    const World::position_t area;
    control_t miner;
  };
  // It is always forward iterated, element is added to end, element is removed from any position.
  std::list<task_t> _tasks;

  operation_t dispatch() noexcept;
  operation_t checkpoint() noexcept;
  operation_t run() noexcept;
  operation_t unload(unit_carrier_t& unit) noexcept;
};
