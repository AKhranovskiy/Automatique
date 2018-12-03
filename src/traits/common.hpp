#pragma once

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

template <class Required, class Given> Required& require(Given& given) {
  Required* r = safe_cast<Required>(given);
  if (nullptr == r) throw ex_trait_wrong_object_type("Wrong object type");
  return *r;
}

template <class ControlBlock, class Object, class Extension, class Operation, class... Args>
ControlBlock& create_operation(Extension& ext, Operation&& op, Args... args) {
  auto& control_block = static_cast<ControlBlock&>(ext);
  Object& obj = require<Object>(control_block.object());
  control_block.add(op(obj, args...));
  return control_block;
}
