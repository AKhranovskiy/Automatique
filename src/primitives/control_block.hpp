#pragma once

#include "primitives/operation.h"
#include "primitives/operation_queue.hpp"

template <class Object, class... Traits>
class control_block_t : public operation_queue_t<Object, operation_t>,
                        public Traits::template extension<control_block_t<Object, Traits...>>... {

  using queue_t = operation_queue_t<Object, operation_t>;

public:
  explicit control_block_t(Object& object) noexcept : queue_t(object) {}
  inline auto finish() noexcept { return queue_t::idle(); }
};
