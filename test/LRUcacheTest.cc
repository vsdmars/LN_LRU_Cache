/**
 * Unit Test for LRUCache with type:
 *
 * key type: IpAddress
 * value type: CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>>
 */
#include <lrucache_common.h>

using namespace testing;

/**
 * Init. LRUCache with 2M entries
 */
class LRUCache2MTest : public Test {
protected:
  constexpr static int LRUC_SIZE = 1'885'725;
  constexpr static int EXPIRYTS = 42;

  // IPv4 a.b.c.d with 'a' stick to 192 and 'b', 'c', 'd' has the range [from,to)
  constexpr static int bfrom{0};
  constexpr static int bto{29};
  constexpr static int cfrom{0};
  constexpr static int cto{255};
  constexpr static int dfrom{0};
  constexpr static int dto{255};

  std::random_device rd{};
  std::mt19937 gen{rd()};

  IPLRUCache lruc{LRUC_SIZE};

protected:
  void SetUp() override { ipJob(lruc, bfrom, bto, cfrom, cto, dfrom, dto, EXPIRYTS); }
  void TearDown() override { lruc.clear(); }
};

/**
 * Single thread access LRU cache test.
 */
TEST_F(LRUCache2MTest, TestSingleThread) {
  constexpr const char* EVICTED_IP = "192.0.0.0";
  constexpr const char* NOT_EVICTED_IP = "192.0.0.1";

  ASSERT_EQ(LRUC_SIZE, lruc.size()) << "cache.size() result not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "cache.capacity() result not match";

  // random generator
  std::uniform_int_distribution<> rangeB{0, 28};
  std::uniform_int_distribution<> rangeCD{1, 254};
  std::uniform_int_distribution<> rangeFalseB{29, 254};

  IPLRUCache::ConstAccessor ca;
  std::stringstream randomFalseIPv4;
  randomFalseIPv4 << "192." << rangeFalseB(gen) << "." << rangeCD(gen) << "." << rangeCD(gen);
  EXPECT_FALSE(lruc.find(ca, create_IpAddress(randomFalseIPv4.str())))
      << "IP [" << randomFalseIPv4.str() << "] shouldn't be found in lru-cache";

  std::stringstream randomIPv4;
  randomIPv4 << "192." << rangeB(gen) << "." << rangeCD(gen) << "." << rangeCD(gen);
  EXPECT_TRUE(lruc.find(ca, create_IpAddress(randomIPv4.str())))
      << "IP [" << randomIPv4.str() << "] can't be found in lru-cache";
  EXPECT_EQ(EXPIRYTS, (*ca).expiryTs);

  lruc.insert(create_IpAddress(randomFalseIPv4.str()), create_cache_value(EXPIRYTS));
  EXPECT_FALSE(lruc.find(ca, create_IpAddress(EVICTED_IP)));
  EXPECT_TRUE(lruc.find(ca, create_IpAddress(NOT_EVICTED_IP)));

  auto eraseResult = lruc.erase(create_IpAddress(randomFalseIPv4.str()));
  EXPECT_EQ(1, eraseResult);
  EXPECT_EQ(LRUC_SIZE - 1, lruc.size());

  lruc.clear();
  EXPECT_FALSE(lruc.find(ca, create_IpAddress(NOT_EVICTED_IP))) << "LRU cache cleared but IP key still can be found";
  EXPECT_EQ(0, lruc.size()) << "LRU cache cleared but size still show not 0";

  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "cache.capacity() result not match";
}

/**
 * multi-threads access LRU cache test.
 *
 * Adding 192 new IPs into a fully filled cache concurrently.
 */
TEST_F(LRUCache2MTest, TestMultiThread_1) {
  constexpr const char* EVICTED_IP = "192.0.0.0";

  ASSERT_EQ(LRUC_SIZE, lruc.size()) << "cache.size() result not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "cache.capacity() result not match";

  // random generator
  std::uniform_int_distribution<> rangeB{0, 28};
  std::uniform_int_distribution<> rangeCD{1, 254};
  std::uniform_int_distribution<> rangeParallelB{29, 254};

  constexpr int ipCnt = 192;
  // 192 random new IPs to be inserted into a filled cache.
  std::unordered_set<std::string> randomIPs{ipCnt};

  auto i = 0;
  while (i < ipCnt) {
    std::stringstream randomIPv4;
    randomIPv4 << "192." << rangeParallelB(gen) << "." << rangeCD(gen) << "." << rangeCD(gen);
    randomIPs.insert(randomIPv4.str());

    ++i;
  }

  // concurrent insert/read from LRUCache
  tbb::parallel_for_each(begin(randomIPs), end(randomIPs), [this](auto item) {
    IPLRUCache::ConstAccessor ca;
    // insert IP concurrently
    lruc.insert(create_IpAddress(item), create_cache_value(EXPIRYTS));
    EXPECT_TRUE(lruc.find(ca, create_IpAddress(item)));

    auto eraseResult = lruc.erase(create_IpAddress(item));
    EXPECT_FALSE(lruc.find(ca, create_IpAddress(item)));
    EXPECT_EQ(1, eraseResult);

    EXPECT_FALSE(lruc.find(ca, create_IpAddress(item)));
  });

  IPLRUCache::ConstAccessor ca;
  EXPECT_FALSE(lruc.find(ca, create_IpAddress(EVICTED_IP)));

  EXPECT_GE(LRUC_SIZE, lruc.size()) << "cache.size() result not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "cache.capacity() result not match";
}

/**
 * multi-threads access LRU cache test.
 *
 * 1. Flush out LRUC_SIZE (1'885'725) IPs and filled with new ones.
 * 2. Read IPs concurrently during the flush out.
 */
TEST_F(LRUCache2MTest, TestMultiThread_2) {
  ASSERT_EQ(LRUC_SIZE, lruc.size()) << "cache.size() result not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "cache.capacity() result not match";

  constexpr int rbfrom = 29;
  constexpr int rbto = 58;
  std::unordered_set<std::string> flushOutIPs{LRUC_SIZE};

  // fill the container flushOutIPs with new IPs.
  ipJob(flushOutIPs, rbfrom, rbto, cfrom, cto, dfrom, dto, EXPIRYTS);

  auto si = std::begin(flushOutIPs);
  auto ei = std::end(flushOutIPs);
  int ipCnt = 0;

  // processing pipeline
  // stage 1. pipe IPs in sequence.
  // stage 2. insert IPs into LRUCache in parallel.
  // stage 3. IPs lookup in LRUCache while insert continues to happen.
  // stage 4. erase IPs in LRUCache while insert/lookup continues to happen.
  // stage 5. IPs lookup in LRUCache while insert/lookup/erase continues to happen.
  // stage 6. update total IP insertion count.
  tbb::parallel_pipeline(
      std::thread::hardware_concurrency() * 2,
      // stage 1.
      tbb::make_filter<void, std::string>(tbb::filter::serial, [&](tbb::flow_control& fc) -> std::string {
        if (si != ei) {
          return *si++;
        } else {
          fc.stop();
          return "";
        }
        // stage 2.
      }) & tbb::make_filter<std::string, std::string>(tbb::filter::parallel, [this](auto ipv4) -> std::string {
        lruc.insert(create_IpAddress(ipv4), create_cache_value(EXPIRYTS));
        return ipv4;
        // stage 3.
      }) & tbb::make_filter<std::string, std::string>(tbb::filter::parallel, [this](auto ipv4) -> std::string {
        IPLRUCache::ConstAccessor ca;
        bool result = lruc.find(ca, create_IpAddress(ipv4));

        EXPECT_TRUE(result) << "IP: [" << ipv4 << "] not found";

        return ipv4;
        // stage 4.
      }) & tbb::make_filter<std::string, std::string>(tbb::filter::parallel, [this](auto ipv4) -> std::string {
        auto result = lruc.erase(create_IpAddress(ipv4));

        EXPECT_EQ(1, result) << "IP: [" << ipv4 << "] not erased";

        return ipv4;
        // stage 5.
      }) & tbb::make_filter<std::string, int>(tbb::filter::parallel, [this](auto ipv4) -> int {
        IPLRUCache::ConstAccessor ca;
        bool result = lruc.find(ca, create_IpAddress(ipv4));

        EXPECT_FALSE(result) << "IP: [" << ipv4 << "] found after erase";

        return 1;
        // stage 6.
      }) & tbb::make_filter<int, void>(tbb::filter::serial, [&ipCnt](auto cnt) { ipCnt += cnt; }));

  EXPECT_GE(LRUC_SIZE, lruc.size()) << "cache.size() result not match";
  ASSERT_EQ(LRUC_SIZE, ipCnt) << "IP count not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "cache.capacity() result not match";
}
