#pragma once

#include "primitives/operation.h"
#include "traits/common.hpp"
#include "units/basic.h"
#include "world.h"

struct trait_move_t {
  template <class ControlBlock> struct extension {
    ControlBlock& move(World::path_t path) {
      return create_operation<ControlBlock, unit_basic_t>(*this, &operation, path);
    }
  };

private:
  static operation_t operation(unit_basic_t& unit, World::path_t path) noexcept;
};

// Trait move exceptions
class ex_trait_move_wrong_position : public std::invalid_argument {
public:
  using std::invalid_argument::invalid_argument;
  explicit ex_trait_move_wrong_position() : std::invalid_argument("Wrong position") {}
};

class ex_trait_move_too_far : public std::invalid_argument {
public:
  using std::invalid_argument::invalid_argument;
  explicit ex_trait_move_too_far() : std::invalid_argument("Next point is too far") {}
};
