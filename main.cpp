#include "cxhash.hpp"
#include "position.hpp"
#include "version.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

template <coord_t W, coord_t H>
std::ostream& operator<<(std::ostream& os, const position_t<W, H>& pos)
{
  return (os << '(' << pos.x << ',' << pos.y << ')');
}

constexpr const coord_t WIDTH{5u};
constexpr const coord_t HEIGHT{5u};
using pos_t = position_t<WIDTH, HEIGHT>;

int main()
{
  std::cout << KVersion << '\n';
  std::cout << "Start game loop" << std::endl;

  assert(2 == distance(pos_t{0, 0}, pos_t{1, 1}));
  assert(2 == distance(pos_t{0, 0}, pos_t{4, 4}));
  assert(1 == distance(pos_t{0, 0}, pos_t{4, 0}));

  for (const auto& p : findPath(pos_t{0, 0}, pos_t{4, 4})) {
    std::cout << p;
  }
  // bool quitRequested = false;
  // while (!quitRequested) {
  //   while (std::string::traits_type::not_eof(std::cin.peek())) {
  //     if (std::cin.get() == 'q') {
  //       std::cout << "Quit\n";
  //       quitRequested = true;
  //       break;
  //     }
  //   }
  // }

  std::cout << "Finish game loop\n"
            << std::endl;
  return 0;
}
