#include <benchmark/benchmark.h>

#include <common_test.h>

using IPVec = std::vector<std::tuple<IpAddress, TimedEntityLookupInfo>>;
// will be init. inside the benchmark functions.
SCALE_IPLRUCache* slruc;
IPVec* randomIPs;

// init. random device.
std::random_device rd{};
std::mt19937 gen{rd()};

// thread count (depends on hardware)
constexpr size_t tcnt = 16;

/**
 * Benchmark for ScalableLRUCache find and insert in each thread.
 */
static void BM_ScalableLRUCacheConcurrentFindInsert_1(benchmark::State& state) {
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
    slruc = new SCALE_IPLRUCache{LRUC_SIZE};
    randomIPs = new IPVec;
    // init. random ip vector
    ipJob(*randomIPs, bfrom, bto, cfrom, cto, dfrom, dto, EXPIRYTS);
  }

  for (auto _ : state) {
    state.PauseTiming();
    auto idx1 = pick(gen);
    auto idx2 = pick(gen);
    state.ResumeTiming();

    SCALE_IPLRUCache::ConstAccessor ca;
    slruc->insert(std::get<0>((*randomIPs)[idx1]), std::get<1>((*randomIPs)[idx1]));
    slruc->find(ca, std::get<0>((*randomIPs)[idx2]));
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete slruc;
  }
}
BENCHMARK(BM_ScalableLRUCacheConcurrentFindInsert_1)
    ->Name("[concurrent] Scalable LRU Cache Find/Insert in each Thread")
    ->Threads(tcnt);

/**
 * Benchmark for ScalableLRUCache find and insert in different thread.
 */
static void BM_ScalableLRUCacheConcurrentFindInsert_2(benchmark::State& state) {
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
    slruc = new SCALE_IPLRUCache{LRUC_SIZE};
    randomIPs = new IPVec;
    // init. random ip vector
    ipJob(*randomIPs, bfrom, bto, cfrom, cto, dfrom, dto, EXPIRYTS);
  }

  for (auto _ : state) {
    state.PauseTiming();
    auto idx1 = pick(gen);
    state.ResumeTiming();

    if (state.iterations() % 2) {
      IPLRUCache::ConstAccessor ca;
      slruc->find(ca, std::get<0>((*randomIPs)[idx1]));
    } else {
      slruc->insert(std::get<0>((*randomIPs)[idx1]), std::get<1>((*randomIPs)[idx1]));
    }
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete slruc;
  }
}
BENCHMARK(BM_ScalableLRUCacheConcurrentFindInsert_2)
    ->Name("[concurrent] Scalable LRU Cache Find/Insert in different Thread")
    ->Threads(tcnt);

/**
 * Benchmark for ScalableLRUCache find in different thread.
 */
static void BM_ScalableLRUCacheConcurrentFind_1(benchmark::State& state) {
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
    slruc = new SCALE_IPLRUCache{LRUC_SIZE};
    randomIPs = new IPVec;
    // init. random ip vector
    ipJob(*randomIPs, bfrom, bto, cfrom, cto, dfrom, dto, EXPIRYTS);
  }

  for (auto _ : state) {
    state.PauseTiming();
    auto idx1 = pick(gen);
    state.ResumeTiming();

    IPLRUCache::ConstAccessor ca;
    slruc->find(ca, std::get<0>((*randomIPs)[idx1]));
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete slruc;
  }
}
BENCHMARK(BM_ScalableLRUCacheConcurrentFind_1)
    ->Name("[concurrent] Scalable LRU Cache Find in different Thread")
    ->Threads(tcnt);

/**
 * Benchmark for ScalableLRUCache insert in different thread.
 */
static void BM_ScalableLRUCacheConcurrentInsert_1(benchmark::State& state) {
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
    slruc = new SCALE_IPLRUCache{LRUC_SIZE};
    randomIPs = new IPVec;
    // init. random ip vector
    ipJob(*randomIPs, bfrom, bto, cfrom, cto, dfrom, dto, EXPIRYTS);
  }

  for (auto _ : state) {
    state.PauseTiming();
    auto idx1 = pick(gen);
    state.ResumeTiming();

    slruc->insert(std::get<0>((*randomIPs)[idx1]), std::get<1>((*randomIPs)[idx1]));
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete slruc;
  }
}
BENCHMARK(BM_ScalableLRUCacheConcurrentInsert_1)
    ->Name("[concurrent] Scalable LRU Cache Insert in different Thread")
    ->Threads(tcnt);

/**
 * Benchmark for ScalableLRUCache insert in sequential.
 */
static void BM_ScalableLRUCacheInsert_1(benchmark::State& state) {
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
    slruc = new SCALE_IPLRUCache{LRUC_SIZE};
    randomIPs = new IPVec;
    // init. random ip vector
    ipJob(*randomIPs, bfrom, bto, cfrom, cto, dfrom, dto, EXPIRYTS);
  }

  for (auto _ : state) {
    state.PauseTiming();
    auto idx1 = pick(gen);
    state.ResumeTiming();

    slruc->insert(std::get<0>((*randomIPs)[idx1]), std::get<1>((*randomIPs)[idx1]));
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete slruc;
  }
}
BENCHMARK(BM_ScalableLRUCacheInsert_1)->Name("Scalable LRU Cacue Insert in sequential");

/**
 * Benchmark for ScalableLRUCache find in sequential.
 */
static void BM_ScalableLRUCacheFind_1(benchmark::State& state) {
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
    slruc = new SCALE_IPLRUCache{LRUC_SIZE};
    randomIPs = new IPVec;
    // init. random ip vector
    ipJob(*randomIPs, bfrom, bto, cfrom, cto, dfrom, dto, EXPIRYTS);
  }

  for (auto _ : state) {
    state.PauseTiming();
    auto idx1 = pick(gen);
    state.ResumeTiming();

    IPLRUCache::ConstAccessor ca;
    slruc->find(ca, std::get<0>((*randomIPs)[idx1]));
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete slruc;
  }
}
BENCHMARK(BM_ScalableLRUCacheFind_1)->Name("Scalable LRU Cache Find in sequential");

/**
 * Benchmark for ScalableLRUCache find/insert/erase in each thread.
 */
static void BM_ScalableLRUCacheConcurrentFindInsertErase_1(benchmark::State& state) {
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
    slruc = new SCALE_IPLRUCache{LRUC_SIZE};
    randomIPs = new IPVec;
    // init. random ip vector
    ipJob(*randomIPs, bfrom, bto, cfrom, cto, dfrom, dto, EXPIRYTS);
  }

  for (auto _ : state) {
    state.PauseTiming();
    auto idx1 = pick(gen);
    auto idx2 = pick(gen);
    state.ResumeTiming();

    SCALE_IPLRUCache::ConstAccessor ca;
    slruc->insert(std::get<0>((*randomIPs)[idx1]), std::get<1>((*randomIPs)[idx1]));
    slruc->find(ca, std::get<0>((*randomIPs)[idx2]));
    slruc->erase(std::get<0>((*randomIPs)[idx2]));
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete slruc;
  }
}
BENCHMARK(BM_ScalableLRUCacheConcurrentFindInsertErase_1)
    ->Name("[concurrent] Scalable LRU Cache Find/Insert/Erase in each Thread")
    ->Threads(tcnt);

/**
 * Benchmark for ScalableLRUCache find/insert/erase in different thread.
 */
static void BM_ScalableLRUCacheConcurrentFindInsertErase_2(benchmark::State& state) {
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
    slruc = new SCALE_IPLRUCache{LRUC_SIZE};
    randomIPs = new IPVec;
    // init. random ip vector
    ipJob(*randomIPs, bfrom, bto, cfrom, cto, dfrom, dto, EXPIRYTS);
  }

  for (auto _ : state) {
    state.PauseTiming();
    auto idx1 = pick(gen);
    state.ResumeTiming();

    switch (state.iterations() % 3) {
      case 0: {
        IPLRUCache::ConstAccessor ca;
        slruc->find(ca, std::get<0>((*randomIPs)[idx1]));
      } break;
      case 1:
        slruc->insert(std::get<0>((*randomIPs)[idx1]), std::get<1>((*randomIPs)[idx1]));
        break;
      case 2:
        slruc->erase(std::get<0>((*randomIPs)[idx1]));
    }
  }

  // cleanup benchmark suite variables.
  if (state.thread_index == 0) {
    delete randomIPs;
    delete slruc;
  }
}
BENCHMARK(BM_ScalableLRUCacheConcurrentFindInsertErase_2)
    ->Name("[concurrent] Scalable LRU Cache Find/Insert/Erase in different Thread")
    ->Threads(tcnt);

BENCHMARK_MAIN();
