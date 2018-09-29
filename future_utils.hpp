#pragma once

#include <chrono>
#include <future>
#include <optional>

template <class R> inline bool is_future_ready(const std::future<R>& f) noexcept {
  constexpr const auto no_wait = std::chrono::milliseconds{0};
  return f.valid() && f.wait_for(no_wait) == std::future_status::ready;
}

template <class R> inline std::optional<R> get_result(std::future<R>&& f) noexcept {
  return is_future_ready(f) ? std::make_optional(f.get()) : std::nullopt;
}

template <class R> inline std::optional<R> get_result(std::future<R>& f) noexcept {
  return get_result(std::forward<std::future<R>>(f));
}

template <class R> std::future<R> make_ready_future(R&& r) noexcept {
  try {
    std::promise<R> p;
    p.set_value(std::forward<R>(r));
    return p.get_future();
  } catch (const std::future_error&) {
    return {};
  } catch (const std::exception&) {
    return {};
  }
}

std::future<void> make_ready_future() noexcept {
  try {
    std::promise<void> p;
    p.set_value();
    return p.get_future();
  } catch (const std::future_error&) {
    return {};
  } catch (const std::exception&) {
    return {};
  }
}
