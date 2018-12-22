#pragma once
#include <functional>
#include <optional>

template <class T> using opt_ref_t = std::optional<std::reference_wrapper<T>>;

template <class T> T& unpack(opt_ref_t<T>& opt) { return opt.value().get(); }

template <class T> opt_ref_t<T> make_opt_ref(T& t) noexcept {
  return std::make_optional(std::ref(t));
}
