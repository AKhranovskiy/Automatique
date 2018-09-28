#pragma once

#include <cstddef> // std::size_t

namespace cx {

template <char ch> constexpr std::size_t to_num() noexcept {
  if constexpr (ch == ' ') {
    return 0u;
  }
  static_assert('0' <= ch && ch <= '9', "Character is not a number");
  return ch - '0';
}

constexpr std::size_t year() noexcept {
  constexpr const char* const s = __DATE__ + 7;
  return to_num<s[0]>() * 1000 + to_num<s[1]>() * 100 + to_num<s[2]>() * 10 + to_num<s[3]>();
}

template <char a, char b, char c> struct month_t;

template <> struct month_t<'S', 'e', 'p'> { static const std::size_t id = 9u; };
// TODO add other months later...

constexpr std::size_t month() noexcept {
  constexpr const char* const s = __DATE__;
  return month_t<s[0], s[1], s[2]>::id;
}

constexpr std::size_t day() noexcept {
  constexpr const char* const s = __DATE__ + 4;
  return to_num<s[0]>() * 10 + to_num<s[1]>();
}

constexpr std::size_t date() noexcept { return year() * 10000 + month() * 100 + day(); };

} // namespace cx
