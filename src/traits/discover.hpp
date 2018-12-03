#pragma once

#include "primitives/operation.h"
#include "traits/common.hpp"
#include "units/basic.h"
#include "world.h"

struct trait_discover_t {
  template <class ControlBlock> struct extension {
    ControlBlock& discover(World::position_t area) noexcept {
      auto& control_block = static_cast<ControlBlock&>(*this);
      const auto& current_pos = control_block.object().position;
      if (current_pos != area) {
        auto path = find_path(current_pos, area);
        control_block.move(path).finish();
      }
      return create_operation<ControlBlock, unit_basic_t>(*this, &operation, area);
    }
  };

private:
  static operation_t operation(const unit_basic_t& unit, World::position_t area) noexcept;
};
