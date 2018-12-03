#pragma once

#include "modules/base_unit_control.h"
#include "primitives/operation.h"
#include "traits/discover.hpp"
#include "traits/move.hpp"
#include "units/basic.h"
#include "world.h"
#include <list>
#include <set>

class module_discovery_t
    : public base_unit_control_t<unit_basic_t, trait_move_t, trait_discover_t> {
public:
  explicit module_discovery_t(module_id_t id, World::position_t position) noexcept;

  result_t discover(distance_t radius) noexcept;

  result_t assign(unit_basic_t& unit) noexcept;
  result_t assign(unit_ref_list_t&& units) noexcept;

private:
  std::set<World::position_t> _areas;

  struct task_t {
    const World::position_t area;
    control_t scout;
  };
  // It is always forward iterated, element is added to end, element is removed from any position.
  std::list<task_t> _tasks;

  operation_t collect_areas(distance_t radius) noexcept;
  operation_t checkpoint() noexcept;
  operation_t dispatch() noexcept;
  operation_t run() noexcept;
};
