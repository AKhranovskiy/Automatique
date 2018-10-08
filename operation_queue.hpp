#pragma once

#include "future_utils.hpp"
#include <future>
#include <queue>

template <class Object, class Operation> struct operation_queue_t {
  using object_t = Object;
  using operation_t = Operation;
  using queue_t = std::queue<operation_t>;
  using idle_promise_t = std::promise<object_t&>;
  using idle_future_t = std::shared_future<object_t&>;

  object_t& _object;
  queue_t _operations;
  idle_promise_t _idle_promise;
  idle_future_t _idle_future;

  explicit operation_queue_t(object_t& object) noexcept : _object{object} {
    _idle_future = _idle_promise.get_future();
  }
  constexpr inline Object& object() noexcept { return _object; }

  operation_t& add(operation_t&& op) noexcept {
    check_idle_promise();
    return _operations.emplace(std::move(op));
  }
  idle_future_t idle() noexcept {
    auto f = _idle_future;
    check_idle_promise();
    return f;
  }

  inline bool operator()() noexcept { return tick(); }
  bool tick() noexcept {
    if (_operations.empty()) return true;

    try {
      if (!_operations.front()()) return false;

      _operations.pop();
      check_idle_promise();
      return _operations.empty();
    } catch (...) {
      _operations = {};
      _idle_promise.set_exception(std::current_exception());
      return true;
    }
  }

  idle_future_t stop() noexcept {
    if (!_operations.empty()) {
      _operations = {};
      _idle_promise.set_value(_object);
    }
    return _idle_future;
  }

private:
  void check_idle_promise() noexcept {
    if (_operations.empty() && !future_utils::is_future_ready(_idle_future)) {
      _idle_promise.set_value(_object);
      _idle_promise = {};
      _idle_future = _idle_promise.get_future();
    }
  }
};
