#include "modules/discovery.h"

module_discovery_t::module_discovery_t(module_id_t id, World::position_t position) noexcept
    : base_unit_control_t{id, position} {}

module_discovery_t::result_t module_discovery_t::discover(distance_t radius) noexcept {
  World::Chronicles().Log(*this) << "has started discovery in radius " << radius << ".\n";
  add(collect_areas(radius));
  add(dispatch());
  return idle();
};

module_discovery_t::result_t module_discovery_t::assign(unit_basic_t& unit) noexcept {
  World::Chronicles().Log(*this) << "has received " << unit << ".\n";
  assign_unit(unit);
  add(dispatch());
  return idle();
}

module_discovery_t::result_t
module_discovery_t::assign(module_discovery_t::unit_ref_list_t&& units) noexcept {
  for (auto unit : units) {
    World::Chronicles().Log(*this) << "has received " << unit << ".\n";
    assign_unit(unit);
  }
  add(dispatch());
  return idle();
}

operation_t module_discovery_t::collect_areas(distance_t radius) noexcept {
  return [radius, this]() mutable -> bool {
    auto unknown_areas = World::get_areas(this->position, radius, ETileContent::Unknown);
    World::Chronicles().Log(*this)
        << "has received " << unknown_areas.size() << " areas to discover.\n";
    this->_areas.insert(unknown_areas.begin(), unknown_areas.end());
    return true;
  };
}

operation_t module_discovery_t::checkpoint() noexcept {
  return [this]() -> bool {
    if (!this->_tasks.empty()) this->add(run());
    if (!this->_areas.empty() && has_idlers()) this->add(dispatch());
    return true;
  };
}

operation_t module_discovery_t::dispatch() noexcept {
  return [this]() mutable -> bool {
    if (!this->_areas.empty() && has_idlers()) {
      auto scout = get_idler();
      auto area =
          std::min_element(this->_areas.begin(), this->_areas.end(),
                           [center = scout.object().position](const auto& lhs, const auto& rhs) {
                             return distance(center, lhs) < distance(center, rhs);
                           });
      assert(area != this->_areas.end());
      scout.start().discover(*area).finish();
      World::Chronicles().Log(*this)
          << "has assigned " << scout.object() << " to discover area" << *area << ".\n";
      this->_tasks.push_back({*area, std::move(scout)});
      this->_areas.erase(area);
    }

    this->add(checkpoint());
    return true;
  };
}

operation_t module_discovery_t::run() noexcept {
  return [this]() mutable -> bool {
    auto& tasks = this->_tasks;
    if (!tasks.empty()) {
      auto completed =
          std::find_if(tasks.begin(), tasks.end(), [](auto& task) { return task.scout(); });
      if (completed != tasks.end()) {
        add_idler(std::move(completed->scout));
        this->_tasks.erase(completed);
      }
    } else {
      World::Chronicles().Log(*this) << "has completed discovery.\n";
    }
    this->add(checkpoint());
    return true;
  };
}
