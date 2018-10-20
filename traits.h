#pragma once

#include "operation.h"
#include "unit.h"
#include "world.h"
#include <exception>
#include <type_traits>

template <class Required, class Given> Required* safe_cast(Given& given) noexcept {
  if constexpr (std::is_base_of_v<Required, Given>)
    return static_cast<Required*>(&given);
  else
    return dynamic_cast<Required*>(&given);
}

// Generic trait exceptions
class ex_trait_wrong_object_type : public std::invalid_argument {
public:
  using std::invalid_argument::invalid_argument;
  explicit ex_trait_wrong_object_type() : std::invalid_argument("Wrong object type") {}
};

template <class ControlBlock, class Object, class Extension, class Operation, class... Args>
ControlBlock& create_operation(Extension& ext, Operation&& op, Args... args) {
  auto& control_block = static_cast<ControlBlock&>(ext);
  Object* obj = safe_cast<Object>(control_block.object());
  if (obj == nullptr) throw ex_trait_wrong_object_type("Wrong object type");
  control_block.add(op(*obj, args...));
  return control_block;
}

struct trait_move_t {
  struct properties {};

  template <class ControlBlock> struct extension {
    ControlBlock& move(World::path_t path) {
      return create_operation<ControlBlock, unit_t>(*this, &operation, path);
    }
  };

private:
  static operation_t operation(unit_t& unit, World::path_t path) noexcept;
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

struct trait_ping_t {
  struct properties {};

  template <class ControlBlock> struct extension {
    ControlBlock& ping() { return create_operation<ControlBlock, unit_t>(*this, &operation); }
  };

private:
  static operation_t operation(const unit_t& unit) noexcept;
};

struct trait_discover_t {
  struct properties {};

  template <class ControlBlock> struct extension {
    ControlBlock& discover(World::position_t area) noexcept {
      auto& control_block = static_cast<ControlBlock&>(*this);
      const auto& current_pos = control_block.object().position;
      if (current_pos != area) {
        auto path = findPath(current_pos, area);
        control_block.move(path).finish();
      }
      return create_operation<ControlBlock, unit_t>(*this, &operation, area);
    }
  };

private:
  static operation_t operation(const unit_t& unit, World::position_t area) noexcept;
};
