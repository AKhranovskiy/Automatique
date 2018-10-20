#include "discovery_module.h"
#include "pretty_print.hpp"
#include <iostream>

discovery_module_t::discovery_module_t(module_id_t id, World::position_t position) noexcept
    : queue_t(*this), id{id}, position{position} {}

discovery_module_t::result_t discovery_module_t::discover(distance_t radius) noexcept {
  std::cout << *this << " has started discovery in radius " << radius << ".\n";
  add(collect_areas(radius));
  add(dispatch());
  return idle();
};

discovery_module_t::result_t discovery_module_t::assign(discovery_module_t::scout_t& scout) {
  std::cout << *this << " has received " << scout.object() << ".\n";
  _idle_scouts.push_back({scout});
  add(dispatch());
  return idle();
}

operation_t discovery_module_t::collect_areas(distance_t radius) noexcept {
  return [radius, this]() mutable -> bool {
    auto unknown_areas = World::get_areas(this->position, radius, ETileContent::Unknown);
    std::cout << *this << " has received " << unknown_areas.size() << " areas to discover.\n";
    this->_areas.insert(unknown_areas.begin(), unknown_areas.end());
    return true;
  };
}

operation_t discovery_module_t::checkpoint() noexcept {
  return [this]() -> bool {
    if (!this->_tasks.empty()) this->add(run());
    if (!this->_areas.empty() && !this->_idle_scouts.empty()) this->add(dispatch());
    return true;
  };
}

operation_t discovery_module_t::dispatch() noexcept {
  return [this]() mutable -> bool {
    if (!this->_areas.empty() && !this->_idle_scouts.empty()) {
      auto& scout = this->_idle_scouts.front();
      auto area = std::min_element(
          this->_areas.begin(), this->_areas.end(),
          [center = scout.get().object().position](const auto& lhs, const auto& rhs) {
            return distance(center, lhs) < distance(center, rhs);
          });
      assert(area != this->_areas.end());
      scout.get().start().discover(*area).finish();
      std::cout << *this << " has assigned " << scout.get().object() << " to discover area" << *area
                << ".\n";
      this->_tasks.push_back({*area, std::move(scout)});
      this->_idle_scouts.pop_front();
      this->_areas.erase(area);
    }

    this->add(checkpoint());
    return true;
  };
}

operation_t discovery_module_t::run() noexcept {
  return [this]() mutable -> bool {
    auto& tasks = this->_tasks;
    if (tasks.empty()) {
      std::cout << *this << " has completed discovery.\n";
    } else {
      auto completed =
          std::find_if(tasks.begin(), tasks.end(), [](auto& task) { return task.scout(); });
      if (completed != tasks.end()) {
        this->_idle_scouts.push_back(std::move(completed->scout));
        this->_tasks.erase(completed);
      }
    }
    this->add(checkpoint());
    return true;
  };
}
