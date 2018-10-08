#include "catch2/catch.hpp"
#include "future_utils.hpp"
#include "operation_queue.hpp"

namespace {
using future_utils::is_future_ready;

using unit_t = std::size_t;
using op_t = std::function<bool()>;
using op_queue = operation_queue_t<unit_t, op_t>;

} // namespace

TEST_CASE("Empty Operation Queue", "[control]") {
  unit_t u{42};
  op_queue q{u};

  SECTION("knows controlled object") { REQUIRE(q.object() == u); }
  SECTION("idles") { REQUIRE(is_future_ready(q.idle())); }
  SECTION("ticks completed") {
    REQUIRE(q());
    REQUIRE(q.tick());
  }
}

TEST_CASE("Operation Queue with operations", "[control]") {
  unit_t u{42};
  op_queue q{u};

  SECTION("executes operations on tick") {
    std::size_t t = 0;
    q.add([&t]() mutable {
      ++t;
      return t == 2;
    });

    REQUIRE(!is_future_ready(q.idle()));
    REQUIRE(!q());
    REQUIRE(!is_future_ready(q.idle()));
    REQUIRE(q());
    REQUIRE(is_future_ready(q.idle()));
    REQUIRE(t == 2);
    REQUIRE(is_future_ready(q.idle()));
  }
  SECTION("catches an exception") {
    q.add([]() {
      throw std::invalid_argument("Throw!");
      return true;
    });
    auto f = q.idle();
    REQUIRE_NOTHROW(q());
    REQUIRE(is_future_ready(f));
    REQUIRE_THROWS_AS(f.get(), std::invalid_argument);
  }
  SECTION("resolves idle futures") {
    std::size_t t = 0;
    q.add([&t]() mutable {
      ++t;
      return t == 2;
    });

    auto idle = q.idle();
    REQUIRE(!is_future_ready(idle));
    REQUIRE(!q());
    auto idle2 = q.idle();
    REQUIRE(!is_future_ready(idle));
    REQUIRE(!is_future_ready(idle2));
    q.add([]() { return true; });
    REQUIRE(!q());
    REQUIRE(!is_future_ready(idle));
    REQUIRE(!is_future_ready(idle2));
    REQUIRE(q());
    REQUIRE(is_future_ready(idle));
    REQUIRE(is_future_ready(idle2));
  }
}

TEST_CASE("Operation Queue", "[control]") {
  unit_t u{42};
  op_queue q{u};

  SECTION("clears queue on stop") {
    q.add([](){return true;});
    auto idle = q.idle();
    REQUIRE(!is_future_ready(idle));
    q.stop();
    REQUIRE(is_future_ready(idle));
    REQUIRE(is_future_ready(q.idle()));
  }
  
}
