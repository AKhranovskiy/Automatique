#include "modules/warehouse.h"
#include "units/carrier.h"
#include "utils/pretty_print.hpp"

module_warehouse_t::module_warehouse_t(entity_id id, World::position_t position) noexcept
    : id{id}, position{position}, _storage{{EResourceType::Coal, {0, 100u}},
                                           {EResourceType::Ore, {0, 100u}},
                                           {EResourceType::Water, {0, 100u}}} {}

operation_t module_warehouse_t::unload(unit_carrier_t& carrier) {
  return [this, &carrier]() mutable -> bool {
    World::Chronicles().Log(*this) << "unloads " << carrier << ".\n";
    if (this->_storage.count(carrier.type) != 1) throw ex_warehouse_unknown_resource{carrier.type};
    if (carrier.volume > 0) {
      auto& storage = this->_storage.at(carrier.type);
      if (storage.current == storage.capacity) throw ex_warehouse_resource_full{carrier.type};
      carrier.volume--;
      storage.current++;
      World::Chronicles().Log(*this) << "unloaded 1 " << carrier.type << ".\n";
    }

    return carrier.volume == 0;
  };
}

std::optional<size_t> module_warehouse_t::get_resource_amount(EResourceType type) const noexcept {
  return _storage.count(type) == 1 ? std::make_optional(_storage.at(type).current) : std::nullopt;
}

std::string to_string(EResourceType type) noexcept {
  std::ostringstream os;
  os << type;
  return os.str();
}
