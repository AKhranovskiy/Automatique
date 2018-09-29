#include "robot.h"
#include "pretty_print.hpp"
#include <iostream>

bool robot_t::move(World::position_t pos) noexcept {
  if (auto dist = distance(_pos, pos); dist != 1) {
    std::cout << "Robot#" << id() << " can not move from " << position() << " to " << pos
              << ". Distance must be 1 but it is " << dist << '\n';
    return false;
  }
  std::cout << "Robot#" << id() << " is moving from " << position() << " to " << pos << '\n';

  _pos = pos;
  return true;
}
