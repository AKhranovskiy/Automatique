#include "modules/mining.h"

module_mining_t::module_mining_t(module_id_t id, World::position_t position) noexcept
    : base_unit_control_t{id, position}, _warehouse{id, position} {}

module_mining_t::result_t module_mining_t::mine(World::position_t pos) noexcept {
  World::Chronicles().Log(*this) << "has started mining area " << pos << ".\n";
  _income_queue.insert(pos);
  add(dispatch());
  return idle();
};

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
    if (!this->_income_queue.empty() && has_idlers()) {
      auto miner = get_idler();
      auto area =
          std::min_element(this->_income_queue.begin(), this->_income_queue.end(),
                           [center = miner.object().position](const auto& lhs, const auto& rhs) {
                             return distance(center, lhs) < distance(center, rhs);
                           });
      assert(area != this->_income_queue.end());
      miner.start().mine(*area).move(find_path(*area, this->_warehouse.position)).finish();
      World::Chronicles().Log(*this)
          << "has assigned " << miner.object() << " to mine area" << *area << ".\n";
      this->_tasks.push_back({*area, std::move(miner)});
      this->_income_queue.erase(area);
    }

    this->add(checkpoint());
    return true;
  };
}

operation_t module_mining_t::checkpoint() noexcept {
  return [this]() -> bool {
    if (!this->_tasks.empty()) this->add(run());
    if (!this->_income_queue.empty() && has_idlers()) this->add(dispatch());
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
        this->add(unload(completed->miner.object()));
        this->_tasks.erase(completed);
      }
    } else {
      World::Chronicles().Log(*this) << "has completed mining.\n";
    }
    this->add(checkpoint());
    return true;
  };
}

operation_t module_mining_t::unload(unit_carrier_t& unit) noexcept {
  World::Chronicles().Log(*this) << "has sent " << unit << " to unload.\n";
  return [this, &unit, task = this->_warehouse.unload(unit)]() mutable -> bool {
    if (task()) {
      this->assign_unit(unit);
      return true;
    }
    return false;
  };
}
