#pragma once

#include "utils/cxdate.hpp"

#include <cstddef> // std::size_t
#include <iosfwd>

struct version_t {
  std::size_t major;
  std::size_t minor;
  std::size_t date;
  const char* const name;
};
static const version_t KVersion = {0, 7, cx::date(), "Mining&Order"};

std::ostream& operator<<(std::ostream& os, const version_t& v) noexcept;
