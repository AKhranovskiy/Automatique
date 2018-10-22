#include "traits.h"
#include <random>

operation_t trait_move_t::operation(unit_t& unit, World::path_t path) noexcept {
  return [&u = unit, p = std::move(path) ]() mutable->bool {
    if (p.empty()) return true;
    if (p.back() != u.position) throw ex_trait_move_wrong_position{};

    p.pop_back();

    if (p.empty()) {
      World::Chronicles().Log(u) << " has arrived to " << u.position << '\n';
      return true;
    }

    auto new_pos = p.back();
    if (distance(new_pos, u.position) > 1) throw ex_trait_move_too_far{};

    World::Chronicles().Log(u) << " is moving from " << u.position << " to " << new_pos << '\n';

    u.position = new_pos;
    return false;
  };
}

operation_t trait_ping_t::operation(const unit_t& unit) noexcept {
  return [&u = unit]()->bool {
    World::Chronicles().Log(u) << " says Pong!\n";
    return true;
  };
}

operation_t trait_discover_t::operation(const unit_t& unit, World::position_t area) noexcept {
  return [&u = unit, area ]()->bool {
    if (World::Tiles[area] == ETileContent::Unknown) {
      std::random_device rd;
      std::ranlux24 gen{rd()};
      std::uniform_int_distribution<> dis(1, get_tile_content_count() - 1);
      World::Tiles[area] = static_cast<ETileContent>(dis(gen));
    }
    World::Chronicles().Log(u) << " has discovered area " << area << ". It contains "
                               << World::Tiles[area] << ".\n";
    return true;
  };
}
