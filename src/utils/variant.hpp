#pragma once

#include <type_traits>
#include <variant>

template <class T, class U> struct is_one_of;
template <class T, class... Ts>
struct is_one_of<T, std::variant<Ts...>> : std::bool_constant<(std::is_same_v<T, Ts> || ...)> {};
template <class V, class T> inline constexpr bool variant_has_v = is_one_of<T, V>::value;

static_assert(variant_has_v<std::variant<int, double, char, int*>, int>);
static_assert(variant_has_v<std::variant<int, double, char, int*>, double>);
static_assert(variant_has_v<std::variant<int, double, char, int*>, char>);
static_assert(variant_has_v<std::variant<int, double, char, int*>, int*>);
static_assert(!variant_has_v<std::variant<int, double, char, int*>, double*>);
static_assert(!variant_has_v<std::variant<int, double, char, int*>, char*>);
