#include "traits/discover.hpp"

operation_t trait_discover_t::operation(const unit_basic_t& unit, World::position_t area) noexcept {
  return [&u = unit, area]() -> bool {
    const auto tile = World::get_tile_info(area);
    World::Chronicles().Log(u) << "has discovered area " << area << "."
                               << " It contains " << tile << ".\n";
    return true;
  };
}
