#pragma once

#include "primitives/operation.h"
#include "traits/common.hpp"
#include "types/resourcetype.h"
#include "units/carrier.h"
#include "world.h"

class ex_trait_mine_no_resource : public std::logic_error {
public:
  using std::logic_error::logic_error;
  explicit ex_trait_mine_no_resource() : std::logic_error("No resource to mine") {}
};

struct trait_mine_t {
  template <class ControlBlock> struct extension {
    ControlBlock& mine(World::position_t area) noexcept {
      auto& control_block = static_cast<ControlBlock&>(*this);
      const auto& current_pos = control_block.object().position;
      if (current_pos != area) {
        auto path = find_path(current_pos, area);
        control_block.move(path).finish();
      }
      return create_operation<ControlBlock>(*this, &operation_mine, area);
    }
  };

private:
  static operation_t operation_mine(unit_carrier_t& miner, World::position_t area) noexcept;
};
