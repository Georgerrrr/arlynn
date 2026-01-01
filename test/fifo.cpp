#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "core/fifo.h"

using value_t = int;
using test_t = core::fifo_t<value_t>;

static constexpr auto fifo_length = 4;

TEST_CASE("Fifo initial conditions") {
  test_t f(fifo_length);

  CHECK(f.capacity() == 4);
  CHECK(f.size() == 0);
  CHECK(f.empty() == true);
  CHECK(f.full() == false);
}

TEST_CASE("Fifo push") {
  test_t f(fifo_length);

  CHECK(f.capacity() == 4);
  CHECK(f.size() == 0);
  CHECK(f.empty() == true);
  CHECK(f.full() == false);

  CHECK(f.push(0) == true);
  CHECK(f.size() == 1);
  CHECK(f.empty() == false);
  CHECK(f.full() == false);

  CHECK(f.push(1) == true);
  CHECK(f.size() == 2);
  CHECK(f.empty() == false);
  CHECK(f.full() == false);

  CHECK(f.push(2) == true);
  CHECK(f.size() == 3);
  CHECK(f.empty() == false);
  CHECK(f.full() == false);

  CHECK(f.push(3) == true);
  CHECK(f.size() == 4);
  CHECK(f.empty() == false);
  CHECK(f.full() == true);

  CHECK(f.push(4) == false);
  CHECK(f.size() == 4);
  CHECK(f.empty() == false);
  CHECK(f.full() == true);
}

TEST_CASE("Fifo pop") {
  test_t f(fifo_length);
  value_t value;

  CHECK(!f.pop());

  for (auto i = 0 ; i < 4 ; i++) {
    f.push(i);
  }

  CHECK(f.full() == true);

  for (auto i = 0 ; i < 4 ; i++) {
    CHECK(f.size() == f.capacity() - i);
    auto v = f.pop();
    CHECK(v);
    CHECK(*v == i);
  }

  CHECK(!f.pop());

  CHECK(f.capacity() == 4);
  CHECK(f.size() == 0);
  CHECK(f.empty() == true);
  CHECK(f.full() == false);
}

