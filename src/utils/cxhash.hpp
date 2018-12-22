#pragma once

#include <cstddef> // std::size_t
#include <cstdint> // std::*_t

namespace cx {

template <std::size_t N> constexpr std::uint64_t FNVhash(const std::uint8_t (&data)[N]) {
  constexpr std::uint64_t basis = 0xcbf29ce484222325;
  constexpr std::uint64_t prime = 0x100000001b3;
  std::uint64_t hash = basis;
  for (auto i = 0u; i < N; ++i) {
    hash ^= data[i];
    hash *= prime;
  }
  return hash;
}

template <typename T> constexpr std::uint64_t FNVhash(T a, T b) {
  using data_t = std::uint8_t[sizeof(T) * 2];

  const T buf[2] = {a, b};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const data_t& ref = *reinterpret_cast<const data_t*>(&buf);
  return FNVhash(ref);
}
} // namespace cx
