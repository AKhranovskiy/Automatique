#pragma once

#include "types/position.hpp"
#include "types/resourcetype.h"
#include "types/tilecontent.h"

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

struct unit_basic_t;
std::ostream& operator<<(std::ostream& os, const unit_basic_t& unit);
struct unit_carrier_t;
std::ostream& operator<<(std::ostream& os, const unit_carrier_t& carrier);

std::ostream& operator<<(std::ostream& os, ETileContent content);

class module_discovery_t;
std::ostream& operator<<(std::ostream& os, const module_discovery_t& discovery);

class module_mining_t;
std::ostream& operator<<(std::ostream& os, const module_mining_t& mining);

class module_warehouse_t;
std::ostream& operator<<(std::ostream& os, const module_warehouse_t& warehouse);

struct World;
std::ostream& operator<<(std::ostream& os, const World& world);

template <class V> std::ostream& operator<<(std::ostream& os, const std::vector<V>& vec) {
  os << '[';
  for (const auto& v : vec) os << v;
  os << ']';
  return os;
}

std::ostream& operator<<(std::ostream& os, EResourceType type);
