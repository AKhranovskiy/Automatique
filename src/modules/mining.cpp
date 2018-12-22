#include "modules/mining.h"

namespace {
template <class Container> // TODO add contract
typename Container::const_iterator find_closest(const Container& cont,
                                                const World::position_t& pos) noexcept {
  return std::min_element(cont.cbegin(), cont.cend(), [&pos](const auto& lhs, const auto& rhs) {
    return distance(pos, lhs) < distance(pos, rhs);
  });
}
} // namespace

module_mining_t::module_mining_t(entity_id id, World::position_t position) noexcept
    : base_unit_control_t{id, position}, _warehouse{id, position} {}

module_mining_t::result_t module_mining_t::mine(World::position_t pos, size_t amount) noexcept {
  World::Chronicles().Log(*this) << "has started mining " << pos << " for " << amount << ".\n";
  _orders.emplace_back(pos, amount);
  add(dispatch());
  return idle();
};

module_mining_t::result_t module_mining_t::mine(ETileContent what, size_t amount,
                                                deferred_t /*tag*/) noexcept {
  World::Chronicles().Log(*this) << "has received deferred order to mine " << amount << " of "
                                 << what << ".\n";

  if (const auto areas = World::get_areas(what); !areas.empty()) {
    auto closest = find_closest(areas, position);
    return mine(*closest, amount);
  }

  World::Chronicles().Log(*this) << "place deferred order for " << amount << "# " << what << ".\n";
  _deferred_orders.emplace_back(what, amount);
  add(dispatch());
  return idle();
}

module_mining_t::result_t module_mining_t::mine(ETileContent what, size_t amount,
                                                immediate_t /*tag*/) noexcept {
  World::Chronicles().Log(*this) << "has received immediate order to mine " << amount << " of "
                                 << what << ".\n";
  if (const auto areas = World::get_areas(what); !areas.empty()) {
    auto closest = find_closest(areas, position);
    return mine(*closest, amount);
  }

  World::Chronicles().Log(*this) << "has not found any place to mine " << what
                                 << ". Order is discarded!\n";
  return idle();
}

module_mining_t::result_t module_mining_t::assign(unit_carrier_t& unit) noexcept {
  World::Chronicles().Log(*this) << "has received " << unit << ".\n";
  assign_unit(unit);
  add(dispatch());
  return idle();
}

module_mining_t::result_t module_mining_t::assign(
    std::initializer_list<std::reference_wrapper<unit_carrier_t>>&& units) noexcept {
  for (auto unit : units) {
    World::Chronicles().Log(*this) << "has received " << unit << ".\n";
    assign_unit(unit);
  }
  add(dispatch());
  return idle();
}

operation_t module_mining_t::dispatch() noexcept {
  return [this]() mutable -> bool {
    if (!this->_deferred_orders.empty()) {
      World::Chronicles().Log(*this) << "checking deferred orders\n";
      if (const auto areas = World::get_areas(this->_deferred_orders.front().type);
          !areas.empty()) {
        const auto closest = find_closest(areas, position);
        const auto amount = this->_deferred_orders.front().amount;
        this->_deferred_orders.pop_front();
        this->_orders.emplace_back(*closest, amount);
      }
    }
    if (!this->_orders.empty() && has_idlers()) {
      auto miner = get_idler();
      auto closest_order =
          std::min_element(this->_orders.begin(), this->_orders.end(),
                           [& pos = miner.object().position](const auto& lhs, const auto& rhs) {
                             return distance(pos, lhs.area) < distance(pos, rhs.area);
                           });

      auto path = find_path(closest_order->area, this->_warehouse.position);
      miner.start().mine(closest_order->area).move(path).finish();

      World::Chronicles().Log(*this)
          << "has assigned " << miner.object() << " to mine area" << closest_order->area << ".\n";

      this->_tasks.push_back({*closest_order, std::move(miner)});
      this->_orders.erase(closest_order);
    }

    this->add(checkpoint());
    return true;
  };
}

operation_t module_mining_t::checkpoint() noexcept {
  return [this]() -> bool {
    if (!this->_deferred_orders.empty()) this->add(dispatch());
    if (!this->_tasks.empty()) this->add(run());
    if (!this->_orders.empty() && has_idlers()) this->add(dispatch());
    return true;
  };
}

operation_t module_mining_t::run() noexcept {
  return [this]() mutable -> bool {
    auto& tasks = this->_tasks;
    if (!tasks.empty()) {
      auto completed =
          std::find_if(tasks.begin(), tasks.end(), [](auto& task) { return task.miner(); });
      if (completed != tasks.end()) {
        this->add(unload(completed->order, completed->miner.object()));
        this->_tasks.erase(completed);
      }
    }
    this->add(checkpoint());
    return true;
  };
}

operation_t module_mining_t::unload(module_mining_t::order_t order, unit_carrier_t& unit) noexcept {
  World::Chronicles().Log(*this) << "has sent " << unit << " to unload.\n";
  order.amount = order.amount > unit.volume ? order.amount - unit.volume : 0u;
  return [this, order, &unit, task = this->_warehouse.unload(unit)]() mutable -> bool {
    if (task()) {
      if (order.amount > 0) this->_orders.emplace_back(order);
      this->assign_unit(unit);
      return true;
    }
    return false;
  };
}

std::int32_t module_mining_t::unit_demand() const noexcept {
  auto orders = static_cast<std::int32_t>(_orders.size());
  auto idlers = static_cast<std::int32_t>(idler_count());
  auto tasks = static_cast<std::int32_t>(_tasks.size());
  return orders - idlers - tasks;
};
