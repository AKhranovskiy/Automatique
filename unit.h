#pragma once

#include "world.h"
#include <cassert>

struct unit_t {
  using unit_id = std::size_t;
  static constexpr const unit_id InvalidId{0u};

  const unit_id id;
  World::position_t position;

  constexpr explicit unit_t(unit_id id, World::position_t position) noexcept
      : id{id}, position{position} {
    assert(id != InvalidId);
  }
};
