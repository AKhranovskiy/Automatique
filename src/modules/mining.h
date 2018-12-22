#pragma once

#include "modules/base_unit_control.h"
#include "modules/warehouse.h"
#include "primitives/operation.h"
#include "traits/mine.hpp"
#include "traits/move.hpp"
#include "types/id.h"
#include "types/tags.h"
#include "world.h"
#include <list>

class module_mining_t : public tag_module_t,
                        public base_unit_control_t<unit_carrier_t, trait_move_t, trait_mine_t> {
public:
  explicit module_mining_t(entity_id id, World::position_t position) noexcept;

  struct deferred_t {};
  static constexpr deferred_t deferred;

  struct immediate_t {};
  static constexpr immediate_t immediate;

  result_t mine(World::position_t position, size_t amount) noexcept;
  result_t mine(ETileContent what, size_t amount, deferred_t) noexcept;
  result_t mine(ETileContent what, size_t amount, immediate_t) noexcept;

  result_t assign(unit_carrier_t& unit) noexcept;
  result_t assign(unit_ref_list_t&& units) noexcept;

  const module_warehouse_t& get_warehouse() const noexcept { return _warehouse; }

  std::int32_t unit_demand() const noexcept;

private:
  struct order_t {
    World::position_t area;
    size_t amount;
    constexpr explicit order_t(World::position_t area, size_t amount) noexcept
        : area{area}, amount{amount} {}
  };
  struct deferred_order_t {
    ETileContent type;
    size_t amount;
    constexpr explicit deferred_order_t(ETileContent what, size_t amount) noexcept
        : type{what}, amount{amount} {}
  };
  struct task_t {
    order_t order;
    control_t miner;
  };

  // It is always forward iterated, element is added to end, element is removed from any position.
  std::list<order_t> _orders;
  std::list<deferred_order_t> _deferred_orders;
  std::list<task_t> _tasks;
  std::list<task_t> _unloading;

  module_warehouse_t _warehouse;

  operation_t dispatch() noexcept;
  operation_t checkpoint() noexcept;
  operation_t run() noexcept;
  operation_t unload(order_t order, unit_carrier_t& unit) noexcept;
};
