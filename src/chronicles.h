#pragma once
#include "utils/pretty_print.hpp"
#include <sstream>

class chronicles_t {
  class log_t {
    chronicles_t& _chronicles;
    std::stringstream _stream;

  public:
    explicit log_t(chronicles_t& chronicles);
    ~log_t();

    log_t(log_t&&) = default;
    log_t(const log_t&) = delete;
    log_t& operator=(log_t&&) = default;
    log_t& operator=(const log_t&) = delete;

    template <class Message> inline log_t& operator<<(const Message& message) noexcept {
      _stream << message;
      return *this;
    }
  };

public:
  void Prolog() noexcept;
  void Epilog() noexcept;

  inline log_t Log() noexcept { return log_t(*this); }
  template <class Object> inline log_t Log(const Object& object) noexcept {
    return std::move(Log() << object << " ");
  }

private:
  virtual void print(const std::stringstream& stream) const noexcept;
};

class null_chronicles final : public chronicles_t {
  void print(const std::stringstream& /*stream*/) const noexcept override {}
};
