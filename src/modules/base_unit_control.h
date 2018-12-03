#pragma once

#include "primitives/module_control.hpp"
#include "primitives/operation.h"
#include "primitives/operation_queue.hpp"
#include "world.h"
#include <list>
#include <set>

template <class Unit, class... Traits>
class base_unit_control_t
    : public operation_queue_t<base_unit_control_t<Unit, Traits...>, operation_t> {
protected:
  using queue_t = operation_queue_t<base_unit_control_t<Unit, Traits...>, operation_t>;

public:
  using module_id_t = size_t;
  using result_t = typename queue_t::idle_future_t;

  const module_id_t id;
  const World::position_t position;

  explicit base_unit_control_t(module_id_t id, World::position_t position) noexcept
      : queue_t(*this), id{id}, position{position} {}

  using unit_ref_list_t = std::initializer_list<std::reference_wrapper<Unit>>;

protected:
  using control_t = control_module_t<Unit, Traits...>;

  void assign_unit(Unit& unit) { _idlers.emplace_back(unit); }
  bool has_idlers() const noexcept { return !_idlers.empty(); }
  control_t get_idler() {
    assert(has_idlers());
    auto idler = std::move(_idlers.front());
    _idlers.pop_front();
    return idler;
  }
  void add_idler(control_t&& idler) noexcept { _idlers.push_back(std::move(idler)); }

private:
  // It is always forward iterated, element is added to end, element is removed from any position.
  std::list<control_t> _idlers;
};
