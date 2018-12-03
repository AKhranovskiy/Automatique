#include "version.h"

#include <ostream>

std::ostream& operator<<(std::ostream& os, const version_t& v) noexcept {
  return os << "Automatique-" << v.name << " v" << v.major << '.' << v.minor << '-' << v.date;
}
