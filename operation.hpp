#pragma once

#include <memory>

struct operation_t {
  struct holder_base {
    using ptr = std::unique_ptr<holder_base>;
    virtual ~holder_base() = default;
    virtual bool operator()() noexcept = 0;
  };
  template <class Operation> struct holder : holder_base {
    inline constexpr explicit holder(Operation&& operation) noexcept
        : _operation{std::move(operation)} {}
    ~holder() override = default;
    bool operator()() noexcept override { return _operation.operator()(); }
    Operation _operation;
  };

  operation_t() = default;

  template <class Operation>
  constexpr inline explicit operation_t(Operation&& operation) noexcept
      : _holder{std::make_unique<holder<Operation>>(std::forward<Operation>(operation))} {}
  inline bool operator()() noexcept {
    assert(_holder != nullptr);
    return _holder->operator()();
  }

  holder_base::ptr _holder;
};
