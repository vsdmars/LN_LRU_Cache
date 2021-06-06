#include <benchmark/benchmark.h>

#include "common_test.h"

using IPVec = std::vector<std::tuple<IpAddress, TimedEntityLookupInfo>>;
// will be init. inside the benchmark function.
IPLRUCache* lruc;
IPVec* randomIPs;

// init. random device.
std::random_device rd{};
std::mt19937 gen{rd()};

/**
 * Concurrent Benchmark for LRUCache find and insert.
 */
static void BM_LRUCache(benchmark::State& state) {
  // keep those const variables inside the function and make it as constexpr
  constexpr int LRUC_SIZE = 1'885'725;
  constexpr int bfrom{0};
  constexpr int bto{29};
  constexpr int cfrom{0};
  constexpr int cto{255};
  constexpr int dfrom{0};
  constexpr int dto{255};
  constexpr int EXPIRYTS{42};
  // uniform distribution device
  std::uniform_int_distribution<> pick{0, LRUC_SIZE - 1};

  // init. benchmark suite variables.
  if (state.thread_index == 0) {
    lruc = new IPLRUCache{LRUC_SIZE};
    randomIPs = new IPVec;
    // init. random ip vector
    ipJob(*randomIPs, bfrom, bto, cfrom, cto, dfrom, dto, EXPIRYTS);
  }

  for (auto _ : state) {
    IPLRUCache::ConstAccessor ca;
    lruc->insert(std::get<0>((*randomIPs)[state.iterations()]), std::get<1>((*randomIPs)[state.iterations()]));
    lruc->find(ca, std::get<0>((*randomIPs)[state.iterations() + 42]));
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete lruc;
  }
}
BENCHMARK(BM_LRUCache)->Name("LRUCache multi-thread benchmark")->Threads(32);

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
