#include <cstdio>
#include <cstdlib>
#include <iostream>
namespace cx {

template <char ch>
constexpr size_t to_num()
{
  if constexpr (ch == ' ') {
    return 0u;
  }
  static_assert('0' <= ch && ch <= '9', "Character is not a number");
  return ch - '0';
}
constexpr size_t year()
{
  constexpr const char* const s = __DATE__ + 7;
  return to_num<s[0]>() * 1000 + to_num<s[1]>() * 100 + to_num<s[2]>() * 10 + to_num<s[3]>();
}
template <char a, char b, char c>
struct month_t;

template <>
struct month_t<'S', 'e', 'p'> {
  static const size_t id = 9u;
};

constexpr size_t month()
{
  constexpr const char* const s = __DATE__;
  return month_t<s[0], s[1], s[2]>::id;
}
constexpr size_t day()
{
  constexpr const char* const s = __DATE__ + 4;
  return to_num<s[0]>() * 10 + to_num<s[1]>();
}

constexpr size_t date()
{
  return year() * 10000 + month() * 100 + day();
};

} // namespace cx

struct version_t {
  size_t major{0};
  size_t minor{0};
  size_t date{cx::date()};
  const char* const name{"Automatique"};
};

std::ostream& operator<<(std::ostream& os, const version_t& v) noexcept
{
  return os << v.name << " v" << v.major << '.' << v.minor << '-' << v.date;
}

constexpr version_t KVersion = {0, 1};

int main()
{
  std::cout << KVersion << std::endl;
  return 0;
}
