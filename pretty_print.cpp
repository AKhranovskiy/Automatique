#include "pretty_print.hpp"

std::ostream& operator<<(std::ostream& os, command_move_t::EResult r) noexcept {
  switch (r) {
  case command_move_t::EResult::Ok:
    return os << "Ok";
  case command_move_t::EResult::NotSupported:
    return os << "NotSupported";
  case command_move_t::EResult::Busy:
    return os << "Busy";
  case command_move_t::EResult::WrongPath:
    return os << "WrongPath";
  case command_move_t::EResult::Failed:
    return os << "Failed";
  }
}
