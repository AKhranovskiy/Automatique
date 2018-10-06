#pragma once

#include "control_block.hpp"
#include <queue>
#include <future>

template <class Object, class... Traits> struct control_module_t {
  using object_t = Object;
  using control_block_t = control_block_t<object_t, Traits...>;
  using queue_t = std::queue<control_block_t>;

  using idle_promise_t = std::promise<object_t&>;
  using idle_future_t = std::shared_future<object_t&>;

  object_t& _object;
  queue_t _blocks;
  idle_promise_t _idle_promise;

  explicit control_module_t(object_t& object) noexcept : _object{object} {}
  inline control_block_t& start() noexcept { return _blocks.emplace(_object); }
  idle_future_t idle() noexcept {
    auto f = _idle_promise.get_future();
    check_idle_promise();
    return f;
  }
  inline bool operator()() noexcept { return tick(); }
  bool tick() noexcept {
    const auto empty = [this]() { return _blocks.empty(); };
    if (!empty() && _blocks.front()()) _blocks.pop();
    if (empty()) {
      _idle_promise.set_value(_object);
      _idle_promise = {};
    }
    return empty();
  }
  void check_idle_promise() noexcept {
    if (_blocks.empty()) {
      _idle_promise.set_value(_object);
      _idle_promise = {};
    }
  }
};
