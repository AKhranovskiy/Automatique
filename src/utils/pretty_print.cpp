#include "utils/pretty_print.hpp"
#include "modules/discovery.h"
#include "modules/mining.h"
#include "units/basic.h"
#include "world.h"

std::ostream& operator<<(std::ostream& os, const unit_basic_t& unit) {
  return os << "unit#" << unit.id;
}
std::ostream& operator<<(std::ostream& os, const unit_carrier_t& carrier) {
  return os << "carrier#" << carrier.id;
}

std::ostream& operator<<(std::ostream& os, ETileContent content) {
  switch (content) {
  case ETileContent::Unknown:
    os << ' ';
    break;

  case ETileContent::None:
    os << 'N';
    break;

  case ETileContent::Coal:
    os << 'C';
    break;

  case ETileContent::Ore:
    os << 'O';
    break;

  case ETileContent::Water:
    os << 'W';
    break;
  };
  return os;
}

std::ostream& operator<<(std::ostream& os, const module_discovery_t& discovery) {
  return os << "Discovery#" << discovery.id;
}
std::ostream& operator<<(std::ostream& os, const module_mining_t& mining) {
  return os << "Mining#" << mining.id;
}
std::ostream& operator<<(std::ostream& os, const module_warehouse_t& warehouse) {
  return os << "Warehouse#" << warehouse.id;
}

std::ostream& operator<<(std::ostream& os, const World& /*world*/) {
  os << "\n┏";
  for (auto i = 0u; i < World::WIDTH; ++i) os << "━";
  os << "┓\n";

  for (auto y = 0u; y < World::HEIGHT; ++y) {
    os << "┃";
    for (auto x = 0u; x < World::WIDTH; ++x) {
      const World::position_t pos{x, y};
      if (World::Tiles.count(pos) == 1) {
        os << World::Tiles.at(pos);
      } else {
        os << ETileContent::Unknown;
      }
    }
    os << "┃\n";
  }

  os << "┗";
  for (auto i = 0u; i < World::WIDTH; ++i) os << "━";
  os << "┛";
  return os;
}

std::ostream& operator<<(std::ostream& os, EResourceType type) {
  switch (type) {
  case EResourceType::None:
    return os << "none";
  case EResourceType::Coal:
    return os << "coal";
  case EResourceType::Ore:
    return os << "ore";
  case EResourceType::Water:
    return os << "water";
  }
}
