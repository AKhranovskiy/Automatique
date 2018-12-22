#pragma once

#include "modules/discovery.h"
#include "modules/mining.h"
#include "types/id.h"
#include "types/optref.hpp"
#include "types/tags.h"
#include "units/basic.h"
#include "units/carrier.h"
#include "utils/algorithm.hpp"
#include "world.h"
#include <algorithm>
#include <type_traits>
#include <unordered_set>
#include <variant>

namespace config {
namespace start {
template <class Entity> struct entity { using type = Entity; };
template <class Module> struct module : entity<Module> {};
template <class Unit, std::size_t Amount = 1> struct unit : entity<Unit> {
  static constexpr size_t amount{Amount};
};
} // namespace start
} // namespace config

template <class... Params> class module_core_t {
public:
  explicit module_core_t(World::position_t position, Params&&... params) : position{position} {
    (configure(std::forward<Params>(params)), ...);
  }

  template <class Module> opt_ref_t<Module> module() noexcept {
    auto m = find_if(_modules,
                     [](const auto& element) { return std::holds_alternative<Module>(element); });
    return m ? make_opt_ref(std::get<Module>(unpack(m))) : std::nullopt;
  }

  bool operator()() noexcept {
    return all_of(_modules, [this](auto&& m) {
      return std::visit(
          [this](auto&& module) mutable -> bool {
            this->manage_units(module);
            return module.tick();
          },
          m);
    });
  }

private:
  struct id_generator_t {
    entity_id last_id = InvalidId;
    auto unit() noexcept -> entity_id { return ++last_id; }
    auto module() noexcept -> entity_id { return ++last_id; }
  };

  const World::position_t position;
  id_generator_t ids;

  using units_t = std::variant<unit_basic_t, unit_carrier_t>;
  using modules_t = std::variant<module_discovery_t, module_mining_t>;

  std::vector<units_t> _units;
  std::unordered_set<entity_id> _idle_units;
  std::unordered_set<entity_id> _busy_units;
  std::vector<modules_t> _modules;

  template <class Entity> void configure(Entity&& entity) {
    if constexpr (std::is_base_of_v<tag_module_t, typename Entity::type>) {
      add_module<typename Entity::type>();
    } else if constexpr (std::is_base_of_v<tag_unit_t, typename Entity::type>) {
      add_unit<typename Entity::type>(entity.amount);
    }
  }

  template <class Unit> void add_unit(size_t amount) {
    for (auto i = 0u; i < amount; ++i) {
      const auto id = ids.unit();
      _units.emplace_back(Unit{id, position});
      _idle_units.emplace(id);
    }
  }

  template <class Module> void add_module() {
    _modules.emplace_back(Module{ids.module(), position});
  }

  template <class Module> void manage_units(Module& module) noexcept {
    const auto demand = module.unit_demand();
    World::Chronicles().Log("core") << module << " demands " << demand << " units.\n";

    if (demand > 0) {
      using unit_t = typename std::decay_t<decltype(module)>::unit_t;
      if (auto unit = this->get_idle_unit<unit_t>(); unit) {
        module.assign(unpack(unit));

        World::Chronicles().Log("core")
            << "has assigned " << unpack(unit) << " to " << module << ".\n";
      }
    } else if (demand < 0) {
      auto units = module.unassign_idlers(-demand);
      this->claim_units(units);

      World::Chronicles().Log("core")
          << "has claimed " << units.size() << " units from " << module << "\n";
    }
  }

  template <class Unit> void claim_units(const std::vector<std::reference_wrapper<Unit>>& units) {
    for (const auto& unit : units) {
      const auto id = unit.get().id;
      _busy_units.erase(id);
      _idle_units.emplace(id);
    }
  }

  template <class Unit> opt_ref_t<Unit> get_idle_unit() noexcept {
    if (_idle_units.empty()) return std::nullopt;

    auto id = *_idle_units.begin();
    auto unit = find_if(_units, [id](const auto& u) {
      return std::visit(
          [id](auto&& unit) -> bool {
            return std::is_convertible_v<decltype(unit), Unit> && unit.id == id;
          },
          u);
    });

    if (!unit) return std::nullopt;

    _idle_units.erase(id);
    _busy_units.emplace(id);

    auto u = std::visit([](auto& u) { return std::reference_wrapper<Unit>(static_cast<Unit&>(u)); },
                        unpack(unit));
    return std::make_optional(u);
  }
};
