#include "chronicles.h"
#include "version.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

void chronicles_t::Prolog() noexcept {
  std::stringstream stream;
  stream << KVersion << '\n';
  stream << "Start game loop\n\n";
  print(stream);
}
void chronicles_t::Epilog() noexcept { print(std::stringstream{} << "Complete game loop\n"); }

void chronicles_t::print(const std::stringstream& stream) const noexcept {
  std::cout << stream.str();
}

chronicles_t::log_t::log_t(chronicles_t& chronicles) : _chronicles(chronicles) {
  using clock = std::chrono::system_clock;
  auto now = clock::to_time_t(clock::now());
  _stream << std::put_time(std::localtime(&now), "%F %T") << " ";
}
chronicles_t::log_t::~log_t() { _chronicles.print(_stream); }
