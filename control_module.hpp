#pragma once

#include "control_block.hpp"
#include "operation_queue.hpp"

template <class Object, class... Traits>
class control_module_t : public operation_queue_t<Object, control_block_t<Object, Traits...>> {

  using control_block_t = control_block_t<Object, Traits...>;

public:
  using object_t = std::decay_t<Object>;
  using queue_t = operation_queue_t<Object, control_block_t>;

  explicit control_module_t(Object& object) noexcept : queue_t(object) {}
  inline control_block_t& start() noexcept {
    return queue_t::add(control_block_t{queue_t::object()});
  }
};
