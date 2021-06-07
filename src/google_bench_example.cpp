#include <benchmark/benchmark.h>
#include <iostream>

// test Argument.
// state.range(0) return assigned argument in each test.
// i.e first loop, 42, second loop, 43.
static void BM_testArg(benchmark::State& state) {
  for (auto _ : state) {
    std::cout << state.range(0) << std::endl;
  }
}
// BENCHMARK(BM_testArg)->Arg(42)->Arg(43);

// test Argument_2.
// state.range(1) return assigned 2nd argument in each test. (i.e 128)
static void BM_testArg_2(benchmark::State& state) {
  for (auto _ : state) {
    std::cout << state.range(1) << std::endl;
  }
}
// BENCHMARK(BM_testArg_2)->Args({42, 128});

// test Range Argument.
// state.range(0) return assigned argument in each test.
// i.e first loop, 42, second loop, 43.
static void BM_testRange(benchmark::State& state) {
  for (auto _ : state) {
    std::cout << state.range(0) << std::endl;
  }
}
// BENCHMARK(BM_testRange)->Range(1, 1000);  // multipled by 8 capped on 1000

BENCHMARK_MAIN();
