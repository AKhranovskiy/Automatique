#pragma once

#include "types/id.h"
#include "types/tags.h"
#include "world.h"
#include <cassert>

struct unit_basic_t : tag_unit_t {
  const entity_id id;
  World::position_t position;

  constexpr explicit unit_basic_t(entity_id id, World::position_t position) noexcept
      : id{id}, position{position} {
    assert(id != InvalidId);
  }
};
