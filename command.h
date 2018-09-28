#pragma once

#include "future_combine.hpp"

#include <future>
#include <utility>

using command_t = std::promise<bool>;
using result_t = decltype(std::declval<command_t>().get_future());

inline bool is_command_done(const result_t& result) noexcept { return is_future_ready(result); }

inline result_t make_result(bool value) {
  command_t t;
  t.set_value(value);
  return t.get_future();
}
