#pragma once

#include "operation.h"
#include <future>
#include <queue>

template <class Object, class... Traits>
class control_block_t : public Traits::template extension<control_block_t<Object, Traits...>>... {
  using promise_t = std::promise<Object&>;
  using future_t = std::future<Object&>;
  using queue_t = std::queue<operation_t>;

  promise_t _promise;
  queue_t _operations;
  Object& _object;

public:
  explicit control_block_t(Object& object) noexcept : _object(object) {}
  constexpr inline bool idle() const noexcept { return _operations.empty(); }
  inline future_t finish() { return _promise.get_future(); };
  inline void add(operation_t op) noexcept { _operations.push(op); }
  constexpr inline Object& object() noexcept { return _object; }
  inline bool operator()() noexcept { return tick(); }
  bool tick() noexcept {
    if (!idle() && _operations.front()()) _operations.pop();
    if (idle()) _promise.set_value(_object);
    return idle();
  }
};
