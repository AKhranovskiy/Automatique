#pragma once

#include "types/optref.hpp"
#include <algorithm>

template <class Container> // TODO add container concept
opt_ref_t<typename Container::value_type>
find_if(Container& cont,
        std::function<bool(typename Container::const_reference)>&& predicat) noexcept {
  auto it = std::find_if(cont.begin(), cont.end(), predicat);
  return it != cont.end() ? make_opt_ref(*it) : std::nullopt;
}

template <class Container> // TODO add container concept
bool all_of(Container& cont,
            std::function<bool(typename Container::reference)>&& predicat) noexcept {
  return std::all_of(cont.begin(), cont.end(), predicat);
}
