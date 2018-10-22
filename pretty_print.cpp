#include "pretty_print.hpp"
#include "discovery_module.h"
#include "unit.h"
#include "world.h"

std::ostream& operator<<(std::ostream& os, const unit_t& unit) { return os << "unit#" << unit.id; }

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

std::ostream& operator<<(std::ostream& os, const discovery_module_t& discovery) {
  return os << "Discovery#" << discovery.id;
}

std::ostream& operator<<(std::ostream& os, const World&  /*world*/) {
  os << "\n┏";
  for (auto i = 0u; i < World::WIDTH; ++i) os << "━";
  os << "┓\n";
  for (auto i = 0u; i < World::HEIGHT; ++i) {
    os << "┃";
    for (auto j = 0u; j < World::WIDTH; ++j) {
      const World::position_t pos{i, j};
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
  os << "┛\n";
  os << '\n';
  return os;
}
