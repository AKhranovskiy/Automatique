#pragma once

#include "position.hpp"

#include <optional>
#include <ostream>
#include <tuple>

template <coord_t W, coord_t H>
std::ostream& operator<<(std::ostream& os, const position_t<W, H>& pos) {
  return (os << '(' << pos.x << ',' << pos.y << ')');
}

template <class T> std::ostream& operator<<(std::ostream& os, const std::optional<T>& o) {
  if (o) {
    return os << *o;
  } else {
    return os << "none";
  }
}

// pretty-print a tuple
template <class Tuple, std::size_t... Is>
void print_tuple_impl(std::ostream& os, const Tuple& t, std::index_sequence<Is...>) {
  ((os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

template <class... Args> auto& operator<<(std::ostream& os, const std::tuple<Args...>& t) {
  os << "(";
  print_tuple_impl(os, t, std::index_sequence_for<Args...>{});
  return os << ")";
}

struct unit_t;
std::ostream& operator<<(std::ostream& os, const unit_t& unit);
