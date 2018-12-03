#include "traits/move.hpp"

operation_t trait_move_t::operation(unit_basic_t& unit, World::path_t path) noexcept {
  return [&u = unit, p = std::move(path)]() mutable -> bool {
    if (p.empty()) return true;
    if (p.back() != u.position) throw ex_trait_move_wrong_position{};

    p.pop_back();

    if (p.empty()) {
      World::Chronicles().Log(u) << "has arrived to " << u.position << '\n';
      return true;
    }

    auto new_pos = p.back();
    if (distance(new_pos, u.position) > 1) throw ex_trait_move_too_far{};

    World::Chronicles().Log(u) << "is moving from " << u.position << " to " << new_pos << '\n';

    u.position = new_pos;
    return false;
  };
}
