#include "modules/mining.h"

mining_module_t::mining_module_t(module_id_t id, World::position_t position) noexcept
    : queue_t(*this), id{id}, position{position}, _warehouse{id, position} {}

mining_module_t::result_t mining_module_t::mine(World::position_t pos) noexcept {
  World::Chronicles().Log(*this) << "has started mining area " << pos << ".\n";
  _income_queue.insert(pos);
  add(dispatch());
  return idle();
};

mining_module_t::result_t mining_module_t::assign(mining_module_t::miner_t& miner) noexcept {
  World::Chronicles().Log(*this) << "has received " << miner.object() << ".\n";
  _idle_miners.emplace_back(miner);
  add(dispatch());
  return idle();
}

mining_module_t::result_t mining_module_t::assign(
    std::initializer_list<std::reference_wrapper<mining_module_t::miner_t>>&& miners) noexcept {
  for (auto miner : miners) {
    World::Chronicles().Log(*this) << "has received " << miner.get().object() << ".\n";
    _idle_miners.emplace_back(miner.get());
  }
  add(dispatch());
  return idle();
}

operation_t mining_module_t::dispatch() noexcept {
  return [this]() mutable -> bool {
    if (!this->_income_queue.empty() && !this->_idle_miners.empty()) {
      auto& miner = this->_idle_miners.front();
      auto area = std::min_element(
          this->_income_queue.begin(), this->_income_queue.end(),
          [center = miner.get().object().position](const auto& lhs, const auto& rhs) {
            return distance(center, lhs) < distance(center, rhs);
          });
      assert(area != this->_income_queue.end());
      miner.get().start().mine(*area).move(find_path(*area, this->_warehouse.position)).finish();
      World::Chronicles().Log(*this)
          << "has assigned " << miner.get().object() << " to mine area" << *area << ".\n";
      this->_tasks.push_back({*area, miner});
      this->_idle_miners.pop_front();
      this->_income_queue.erase(area);
    }

    this->add(checkpoint());
    return true;
  };
}

operation_t mining_module_t::checkpoint() noexcept {
  return [this]() -> bool {
    if (!this->_tasks.empty()) this->add(run());
    if (!this->_income_queue.empty() && !this->_idle_miners.empty()) this->add(dispatch());
    return true;
  };
}

operation_t mining_module_t::run() noexcept {
  return [this]() mutable -> bool {
    auto& tasks = this->_tasks;
    if (tasks.empty()) {
      World::Chronicles().Log(*this) << "has completed mining.\n";
    } else {
      auto completed =
          std::find_if(tasks.begin(), tasks.end(), [](auto& task) { return task.miner(); });
      if (completed != tasks.end()) {
        this->add(unload(completed->miner));
        this->_tasks.erase(completed);
      }
    }
    this->add(checkpoint());
    return true;
  };
}

operation_t mining_module_t::unload(mining_module_t::miner_ref_t miner) noexcept {
  World::Chronicles().Log(*this) << "has sent " << miner.get().object() << " to unload.\n";
  return _warehouse.unload(miner.get().object());
}
