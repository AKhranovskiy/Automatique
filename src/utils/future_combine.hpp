#pragma once

#include "utils/future.hpp"

#include <future>
#include <optional>
#include <tuple>

template <class T> auto fwd(T&& t) { return std::forward<std::decay_t<T>>(t); }

template <class... FutureArgs> struct future_wrapper_t;

template <class Op, class Fa, class Fb> class future_combine {
protected:
  Fa _fa;
  Fb _fb;

public:
  using result_t = decltype(std::tuple_cat(std::declval<typename std::decay_t<Fa>::result_t>(),
                                           std::declval<typename std::decay_t<Fb>::result_t>()));

  constexpr explicit future_combine(Fa&& fa, Fb&& fb) : _fa{fwd(fa)}, _fb{fwd(fb)} {}
  explicit inline operator bool() const noexcept { return Op{}(_fa, _fb); }
  inline result_t get() { return *this ? std::tuple_cat(_fa.get(), _fb.get()) : result_t{}; }
};

template <class T> struct value_t {
  using type = T;
  type value;
  constexpr explicit value_t(type&& t) : value{std::forward<T>(t)} {}
};
template <class T> using ref_value_t = value_t<T&>;

template <class T> auto make_value_t(T& t) { return ref_value_t<T>{t}; }
template <class T> auto make_value_t(T&& t) { return value_t<T>{fwd(t)}; }

template <class R, class V> struct future_wrapper_t<std::shared_future<R>, V> {
  using type_t = V;
  using result_t = std::tuple<std::optional<R>>;

  type_t _f;

  constexpr explicit future_wrapper_t(type_t&& f) : _f{fwd(f)} {}
  inline explicit operator bool() const noexcept { return future_utils::is_future_ready(_f.value); }
  inline result_t get() { return {get_result(fwd(_f.value))}; }
};

template <class Op, class Ra, class Rb, class V>
struct future_wrapper_t<future_combine<Op, Ra, Rb>, V> {
  using type_t = V;
  using result_t = typename type_t::type::result_t;

  type_t _fc;

  constexpr explicit future_wrapper_t(type_t&& fc) : _fc{fwd(fc)} {}
  inline explicit operator bool() const noexcept { return static_cast<bool>(_fc.value); }
  inline result_t get() { return _fc.value.get(); }
};

template <class Ra> auto make_future_wrapper(std::shared_future<Ra>& f) noexcept {
  using future_t = std::decay_t<decltype(f)>;
  return future_wrapper_t<future_t, ref_value_t<future_t>>{make_value_t(f)};
}
template <class Ra> auto make_future_wrapper(std::shared_future<Ra>&& f) noexcept {
  using future_t = std::decay_t<decltype(f)>;
  return future_wrapper_t<future_t, value_t<future_t>>{make_value_t(fwd(f))};
}
template <class Op, class Fa, class Fb>
auto make_future_wrapper(future_combine<Op, Fa, Fb>&& fc) noexcept {
  using fc_t = std::decay_t<decltype(fc)>;
  return future_wrapper_t<fc_t, value_t<fc_t>>{make_value_t(fwd(fc))};
}
template <class Op, class FWA, class FWB> auto make_future_combine(FWA&& fwa, FWB&& fwb) noexcept {
  return future_combine<Op, FWA, FWB>{fwd(fwa), fwd(fwb)};
}

template <class A, class B>
constexpr inline auto operator||(std::shared_future<A>& fa, std::shared_future<B>& fb) noexcept {
  return make_future_combine<std::logical_or<>>(make_future_wrapper(fa), make_future_wrapper(fb));
}
template <class A, class B>
constexpr inline auto operator||(std::shared_future<A>& fa, std::shared_future<B>&& fb) noexcept {
  return make_future_combine<std::logical_or<>>(make_future_wrapper(fa),
                                                make_future_wrapper(fwd(fb)));
}
template <class A, class B>
constexpr inline auto operator||(std::shared_future<A>&& fa, std::shared_future<B>& fb) noexcept {
  return make_future_combine<std::logical_or<>>(make_future_wrapper(fwd(fa)),
                                                make_future_wrapper(fb));
}
template <class A, class B>
constexpr inline auto operator||(std::shared_future<A>&& fa, std::shared_future<B>&& fb) noexcept {
  return make_future_combine<std::logical_or<>>(make_future_wrapper(fwd(fa)),
                                                make_future_wrapper(fwd(fb)));
}

template <class Op, class A, class B, class C>
constexpr inline auto operator||(future_combine<Op, A, B>&& fca,
                                 std::shared_future<C>& fb) noexcept {
  return make_future_combine<std::logical_or<>>(make_future_wrapper(fwd(fca)),
                                                make_future_wrapper(fb));
}
template <class Op, class A, class B, class C>
constexpr inline auto operator||(future_combine<Op, A, B>&& fca,
                                 std::shared_future<C>&& fb) noexcept {
  return make_future_combine<std::logical_or<>>(make_future_wrapper(fwd(fca)),
                                                make_future_wrapper(fwd(fb)));
}
template <class Op, class A, class B, class Op2, class A2, class B2>
constexpr inline auto operator||(future_combine<Op, A, B>&& fc,
                                 future_combine<Op2, A2, B2>&& fc2) noexcept {
  return make_future_combine<std::logical_or<>>(make_future_wrapper(fwd(fc)),
                                                make_future_wrapper(fwd(fc2)));
}

template <class A, class B>
constexpr inline auto operator&&(std::shared_future<A>& fa, std::shared_future<B>& fb) noexcept {
  return make_future_combine<std::logical_and<>>(make_future_wrapper(fa), make_future_wrapper(fb));
}
template <class A, class B>
constexpr inline auto operator&&(std::shared_future<A>& fa, std::shared_future<B>&& fb) noexcept {
  return make_future_combine<std::logical_and<>>(make_future_wrapper(fa),
                                                 make_future_wrapper(fwd(fb)));
}
template <class A, class B>
constexpr inline auto operator&&(std::shared_future<A>&& fa, std::shared_future<B>& fb) noexcept {
  return make_future_combine<std::logical_and<>>(make_future_wrapper(fwd(fa)),
                                                 make_future_wrapper(fb));
}
template <class A, class B>
constexpr inline auto operator&&(std::shared_future<A>&& fa, std::shared_future<B>&& fb) noexcept {
  return make_future_combine<std::logical_and<>>(make_future_wrapper(fwd(fa)),
                                                 make_future_wrapper(fwd(fb)));
}
template <class Op, class A, class B, class C>
constexpr inline auto operator&&(future_combine<Op, A, B>&& fca,
                                 std::shared_future<C>& fb) noexcept {
  return make_future_combine<std::logical_and<>>(make_future_wrapper(fwd(fca)),
                                                 make_future_wrapper(fb));
}
template <class Op, class A, class B, class C>
constexpr inline auto operator&&(future_combine<Op, A, B>&& fca,
                                 std::shared_future<C>&& fb) noexcept {
  return make_future_combine<std::logical_and<>>(make_future_wrapper(fwd(fca)),
                                                 make_future_wrapper(fwd(fb)));
}
template <class Op, class A, class B, class Op2, class A2, class B2>
constexpr inline auto operator&&(future_combine<Op, A, B>&& fc,
                                 future_combine<Op2, A2, B2>&& fc2) noexcept {
  return make_future_combine<std::logical_and<>>(make_future_wrapper(fwd(fc)),
                                                 make_future_wrapper(fwd(fc2)));
}
