#pragma once

#include "modules/control.hpp"
#include "primitives/operation.h"
#include "primitives/operation_queue.hpp"
#include "traits/discover.hpp"
#include "traits/move.hpp"
#include "units/basic.h"
#include "world.h"
#include <list>
#include <set>

class discovery_module_t : public operation_queue_t<discovery_module_t, operation_t> {
  using queue_t = operation_queue_t<discovery_module_t, operation_t>;

public:
  using module_id_t = size_t;
  using result_t = queue_t::idle_future_t;

  const module_id_t id;
  const World::position_t position;

  explicit discovery_module_t(module_id_t id, World::position_t position) noexcept;

  result_t discover(distance_t radius) noexcept;

  using scout_t = control_module_t<unit_basic_t, trait_move_t, trait_discover_t>;
  result_t assign(scout_t& scout) noexcept;
  result_t assign(
      std::initializer_list<std::reference_wrapper<discovery_module_t::scout_t>>&& scouts) noexcept;

private:
  std::set<World::position_t> _areas;

  using scout_ref_t = std::reference_wrapper<scout_t>;
  // It is always forward iterated, element is added to end, element is removed from any position.
  std::list<scout_ref_t> _idle_scouts;

  struct task_t {
    const World::position_t area;
    scout_ref_t scout;
  };
  // It is always forward iterated, element is added to end, element is removed from any position.
  std::list<task_t> _tasks;

  operation_t collect_areas(distance_t radius) noexcept;
  operation_t checkpoint() noexcept;
  operation_t dispatch() noexcept;
  operation_t run() noexcept;
};
