#include "pretty_print.hpp"
#include "unit.h"

std::ostream& operator<<(std::ostream& os, const unit_t& unit) {
  return os << "unit#" << unit.id; //<< " " << unit.position;
}
