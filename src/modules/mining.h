#pragma once

#include "modules/control.hpp"
#include "modules/warehouse.h"
#include "primitives/operation.h"
#include "primitives/operation_queue.hpp"
#include "traits/mine.hpp"
#include "traits/move.hpp"
#include "world.h"
#include <list>
#include <set>

class mining_module_t : public operation_queue_t<mining_module_t, operation_t> {
  using queue_t = operation_queue_t<mining_module_t, operation_t>;

public:
  using module_id_t = size_t;
  using result_t = queue_t::idle_future_t;

  const module_id_t id;
  const World::position_t position;

  explicit mining_module_t(module_id_t id, World::position_t position) noexcept;

  result_t mine(World::position_t position) noexcept;

  using miner_t = control_module_t<unit_carrier_t, trait_move_t, trait_mine_t>;
  result_t assign(miner_t& miner) noexcept;

  result_t assign(std::initializer_list<std::reference_wrapper<miner_t>>&& miners) noexcept;

  const warehouse_module_t& get_warehouse() const noexcept { return _warehouse; }

private:
  std::set<World::position_t> _income_queue;
  warehouse_module_t _warehouse;

  using miner_ref_t = std::reference_wrapper<miner_t>;
  // It is always forward iterated, element is added to end, element is removed from any position.
  std::list<miner_ref_t> _idle_miners;

  struct task_t {
    const World::position_t area;
    miner_ref_t miner;
  };
  // It is always forward iterated, element is added to end, element is removed from any position.
  std::list<task_t> _tasks;

  operation_t dispatch() noexcept;
  operation_t checkpoint() noexcept;
  operation_t run() noexcept;
  operation_t unload(miner_ref_t miner) noexcept;
};
