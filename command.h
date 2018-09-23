#pragma once

#include <chrono>
#include <future>
#include <utility>

using command_t = std::promise<bool>;
using result_t = decltype(std::declval<command_t>().get_future());

inline bool is_command_done(const result_t& result) noexcept
{
  constexpr const auto no_wait = std::chrono::milliseconds{0};
  return result.valid() && result.wait_for(no_wait) == std::future_status::ready;
}

result_t make_result(bool value) noexcept
{
  command_t t;
  t.set_value(value);
  return t.get_future();
}
