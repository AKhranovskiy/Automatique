#include "pretty_print.hpp"
#include "discovery_module.h"
#include "unit.h"

std::ostream& operator<<(std::ostream& os, const unit_t& unit) {
  return os << "unit#" << unit.id; //<< " " << unit.position;
}

std::ostream& operator<<(std::ostream& os, ETileContent content) {
  os << "[";
  switch (content) {
  case ETileContent::Unknown:
    os << "Unknown";
    break;

  case ETileContent::None:
    os << "None";
    break;

  case ETileContent::Coal:
    os << "Coal";
    break;

  case ETileContent::Ore:
    os << "Ore";
    break;

  case ETileContent::Water:
    os << "Water";
    break;
  };
  os << "]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const discovery_module_t& discovery) {
  return os << "Discovery#" << discovery.id << discovery.position;
}
