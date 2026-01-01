#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "core/fifo.h"

TEST_CASE("Fifo benchmark") {
  core::fifo_t<int64_t> f(4096);

  BENCHMARK("fifo") {
    auto reader = std::jthread([&] {
        std::optional<int64_t> value;
        for (int64_t i = 0 ; ; i++) {
          while (!(value = f.pop()));

          if (*value == -1) break;
          if (*value != i) {
            throw std::runtime_error("invalid value!");
          }
        }
        });

    for (int64_t val = 0 ; val < 1000000 ; val++) {
      while (f.full());
      f.push(val);
    }

    f.push(-1);

    return 0;
  };
}
