#pragma once
#include "pretty_print.hpp"
#include <sstream>

class chronicles_t {
  class log_t {
    chronicles_t& _chronicles;
    std::stringstream _stream;

  public:
    log_t(chronicles_t& chronicles);
    ~log_t();
    log_t(log_t&&) = default;

    template <class Message> inline log_t& operator<<(const Message& message) noexcept {
      _stream << message;
      return *this;
    }
  };

public:
  void Prolog() noexcept;
  void Epilog() noexcept;

  template <class Object> inline log_t Log(const Object& object) noexcept {
    return std::move(log_t(*this) << object);
  }

private:
  virtual void print(const std::stringstream& stream) const noexcept;
};

class null_chronicles final : public chronicles_t {
  void print(const std::stringstream&) const noexcept override {}
};
