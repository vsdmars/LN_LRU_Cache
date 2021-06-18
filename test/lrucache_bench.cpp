#include <benchmark/benchmark.h>

#include "common_test.h"

using IPVec = std::vector<std::tuple<IpAddress, TimedEntityLookupInfo>>;
// will be init. inside the benchmark functions.
IPLRUCache* lruc;
IPVec* randomIPs;

// init. random device.
std::random_device rd{};
std::mt19937 gen{rd()};

// thread count (depends on hardware)
constexpr size_t tcnt = 16;

/**
 * Benchmark for LRUCache find and insert in each thread.
 */
static void BM_LRUCacheConcurrentFindInsert_1(benchmark::State& state) {
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
    state.PauseTiming();
    auto idx1 = pick(gen);
    auto idx2 = pick(gen);
    state.ResumeTiming();

    IPLRUCache::ConstAccessor ca{};
    lruc->insert(std::get<0>((*randomIPs)[idx1]), std::get<1>((*randomIPs)[idx1]));
    lruc->find(ca, std::get<0>((*randomIPs)[idx2]));
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete lruc;
  }
}
BENCHMARK(BM_LRUCacheConcurrentFindInsert_1)->Name("[concurrent] Find/Insert in each Thread")->Threads(tcnt);

/**
 * Benchmark for LRUCache find and insert in different thread.
 */
static void BM_LRUCacheConcurrentFindInsert_2(benchmark::State& state) {
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
    state.PauseTiming();
    auto idx1 = pick(gen);
    state.ResumeTiming();

    if (state.iterations() % 2) {
      IPLRUCache::ConstAccessor ca{};
      lruc->find(ca, std::get<0>((*randomIPs)[idx1]));
    } else {
      lruc->insert(std::get<0>((*randomIPs)[idx1]), std::get<1>((*randomIPs)[idx1]));
    }
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete lruc;
  }
}
BENCHMARK(BM_LRUCacheConcurrentFindInsert_2)->Name("[concurrent] Find/Insert in different Thread")->Threads(tcnt);

/**
 * Benchmark for LRUCache find in different thread.
 */
static void BM_LRUCacheConcurrentFind_1(benchmark::State& state) {
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
    state.PauseTiming();
    auto idx1 = pick(gen);
    state.ResumeTiming();

    IPLRUCache::ConstAccessor ca{};
    lruc->find(ca, std::get<0>((*randomIPs)[idx1]));
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete lruc;
  }
}
BENCHMARK(BM_LRUCacheConcurrentFind_1)->Name("[concurrent] Find in different Thread")->Threads(tcnt);

/**
 * Benchmark for LRUCache insert in different thread.
 */
static void BM_LRUCacheConcurrentInsert_1(benchmark::State& state) {
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
    state.PauseTiming();
    auto idx1 = pick(gen);
    state.ResumeTiming();

    lruc->insert(std::get<0>((*randomIPs)[idx1]), std::get<1>((*randomIPs)[idx1]));
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete lruc;
  }
}
BENCHMARK(BM_LRUCacheConcurrentInsert_1)->Name("[concurrent] Insert in different Thread")->Threads(tcnt);

/**
 * Benchmark for LRUCache insert in sequential.
 */
static void BM_LRUCacheInsert_1(benchmark::State& state) {
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
    state.PauseTiming();
    auto idx1 = pick(gen);
    state.ResumeTiming();

    lruc->insert(std::get<0>((*randomIPs)[idx1]), std::get<1>((*randomIPs)[idx1]));
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete lruc;
  }
}
BENCHMARK(BM_LRUCacheInsert_1)->Name("Insert in sequential");

/**
 * Benchmark for LRUCache find in sequential.
 */
static void BM_LRUCacheFind_1(benchmark::State& state) {
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
    state.PauseTiming();
    auto idx1 = pick(gen);
    state.ResumeTiming();

    IPLRUCache::ConstAccessor ca{};
    lruc->find(ca, std::get<0>((*randomIPs)[idx1]));
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete lruc;
  }
}
BENCHMARK(BM_LRUCacheFind_1)->Name("Find in sequential");

/**
 * Benchmark for LRUCache find/insert/erase in each thread.
 */
static void BM_LRUCacheConcurrentFindInsertErase_1(benchmark::State& state) {
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
    state.PauseTiming();
    auto idx1 = pick(gen);
    auto idx2 = pick(gen);
    auto idx3 = pick(gen);
    state.ResumeTiming();

    IPLRUCache::ConstAccessor ca{};
    lruc->insert(std::get<0>((*randomIPs)[idx1]), std::get<1>((*randomIPs)[idx1]));
    lruc->find(ca, std::get<0>((*randomIPs)[idx2]));
    // std::cout << "begin\n" << std::flush;
    lruc->erase(std::get<0>((*randomIPs)[idx3]));
    // std::cout << "end\n" << std::flush;
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete lruc;
  }
}
BENCHMARK(BM_LRUCacheConcurrentFindInsertErase_1)->Name("[concurrent] Find/Insert/Erase in each Thread")->Threads(tcnt);

/**
 * Benchmark for LRUCache find/insert/erase in different thread.
 */
static void BM_LRUCacheConcurrentFindInsertErase_2(benchmark::State& state) {
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
    state.PauseTiming();
    auto idx1 = pick(gen);
    state.ResumeTiming();

    switch(state.iterations() % 3) {
      case 0:
        {
          IPLRUCache::ConstAccessor ca{};
          lruc->find(ca, std::get<0>((*randomIPs)[idx1]));
        }
        break;
      case 1:
        lruc->insert(std::get<0>((*randomIPs)[idx1]), std::get<1>((*randomIPs)[idx1]));
        break;
      case 2:
        lruc->erase(std::get<0>((*randomIPs)[idx1]));
    }
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete lruc;
  }
}
BENCHMARK(BM_LRUCacheConcurrentFindInsertErase_2)->Name("[concurrent] Find/Insert/Erase in different Thread")->Threads(tcnt);


BENCHMARK_MAIN();
