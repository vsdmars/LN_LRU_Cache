/**
 * Unit Test for LRUCache with type:
 *
 * key type: IpAddress
 * value type: CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>>
 */
#include "lrucache_common.h"
#include <iostream>

using namespace testing;

/**
 * Init. LRUCache with 255 entries
 */
class LRUCacheTest : public Test {
protected:
  constexpr static int LRUC_SIZE = 255;
  constexpr static int EXPIRYTS = 42;

  // IPv4 a.b.c.d with 'a' stick to 192 and 'b', 'c', 'd' has the range [from,to)
  constexpr static int bfrom{0};
  constexpr static int bto{1};
  constexpr static int cfrom{0};
  constexpr static int cto{1};
  constexpr static int dfrom{0};
  constexpr static int dto{255};

  std::random_device rd{};
  std::mt19937 gen{rd()};

  IPLRUCache lruc{LRUC_SIZE};

protected:
  void SetUp() override { ipJob(lruc, bfrom, bto, cfrom, cto, dfrom, dto, EXPIRYTS); }
  void TearDown() override {}
};

/**
 * Single thread access LRU cache test.
 */
TEST_F(LRUCacheTest, TestSingleThread) {
  constexpr const char* EVICTED_IP = "192.0.0.0";
  constexpr const char* NOT_EVICTED_IP = "192.0.0.1";

  ASSERT_EQ(LRUC_SIZE, lruc.size()) << "cache.size() result not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "cache.capacity() result not match";

  // random generator
  std::uniform_int_distribution<> rangeB{0, 0};
  std::uniform_int_distribution<> rangeC{0, 0};
  std::uniform_int_distribution<> rangeD{1, 254};
  std::uniform_int_distribution<> rangeFalseB{1, 254};

  IPLRUCache::ConstAccessor ca;
  std::stringstream randomFalseIPv4;
  randomFalseIPv4 << "192." << rangeFalseB(gen) << "." << rangeC(gen) << "." << rangeD(gen);
  EXPECT_FALSE(lruc.find(ca, create_IpAddress(randomFalseIPv4.str())))
      << "IP [" << randomFalseIPv4.str() << "] shouldn't be found in lru-cache";

  std::stringstream randomIPv4;
  randomIPv4 << "192." << rangeB(gen) << "." << rangeC(gen) << "." << rangeD(gen);
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
TEST_F(LRUCacheTest, TestMultiThread_1) {
  constexpr const char* EVICTED_IP = "192.0.0.0";

  ASSERT_EQ(LRUC_SIZE, lruc.size()) << "cache.size() result not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "cache.capacity() result not match";

  // random generator
  std::uniform_int_distribution<> rangeC{0, 0};
  std::uniform_int_distribution<> rangeD{0, 254};
  std::uniform_int_distribution<> rangeParallelB{1, 254};

  constexpr int ipCnt = 42;
  // 192 random new IPs to be inserted into a filled cache.
  std::unordered_set<std::string> randomIPs{ipCnt};

  auto i = 0;
  while (i < ipCnt) {
    std::stringstream randomIPv4;
    randomIPv4 << "192." << rangeParallelB(gen) << "." << rangeC(gen) << "." << rangeD(gen);
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
 * 1. Flush out LRUC_SIZE IPs and filled with new ones.
 * 2. Read IPs concurrently during the flush out.
 */
TEST_F(LRUCacheTest, TestMultiThread_2) {
  ASSERT_EQ(LRUC_SIZE, lruc.size()) << "cache.size() result not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "cache.capacity() result not match";

  constexpr int rbfrom = 1;
  constexpr int rbto = 2;
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

/**
 * Test concurrent insert same key
 */
TEST(LRUCacheTest_Same_Key, ConcurrentInsert) {
  auto key = create_IpAddress("192.168.1.1");
  auto value = create_cache_value(42);
  std::array<unsigned char, 100000> data;
  data.fill('o');
  IPLRUCache lruc{42};

  // concurrent insert
  tbb::parallel_for_each(data, [&, key, value](auto _) {
    IPLRUCache::ConstAccessor ca;
    // insert IP concurrently
    lruc.insert(key, value);
  });

  ASSERT_EQ(1, lruc.size()) << "cache.size() result not match";
}

/**
 * Test concurrent erase same key
 */
TEST(LRUCacheTest_Same_Key, ConcurrentErase) {
  auto key = create_IpAddress("192.168.1.1");
  auto value = create_cache_value(42);
  std::array<unsigned char, 100000> data;
  data.fill('o');
  IPLRUCache lruc{42};

  // concurrent insert
  tbb::parallel_for_each(data, [&, key, value](auto _) {
    IPLRUCache::ConstAccessor ca;
    // insert IP concurrently
    lruc.insert(key, value);
  });

  ASSERT_EQ(1, lruc.size()) << "cache.size() result not match";

  // concurrent erase
  tbb::parallel_for_each(data, [&, key](auto _) {
    IPLRUCache::ConstAccessor ca;
    // erase IP concurrently
    lruc.erase(key);
  });

  ASSERT_EQ(0, lruc.size()) << "cache.size() result not match";
}

/**
 * Test concurrent erase same key
 */
TEST(LRUCacheTest_Same_Key, ConcurrentInsertErase) {
  auto key1 = create_IpAddress("192.168.1.1");
  auto key2 = create_IpAddress("192.168.1.2");
  auto value = create_cache_value(42);
  std::array<unsigned char, 100000> data;
  data.fill('o');
  IPLRUCache lruc{42};

  auto si = std::begin(data);
  auto ei = std::end(data);

  tbb::parallel_pipeline(std::thread::hardware_concurrency() * 8,
                         // stage 1, pipe data
                         tbb::make_filter<void, char>(tbb::filter::serial, [&](tbb::flow_control& fc) -> char {
                           if (si != ei) {
                             ++si;
                             return 'o';
                           } else {
                             fc.stop();
                             return 'o';
                           }
                           // stage 2, insert keys.
                         }) & tbb::make_filter<char, char>(tbb::filter::parallel, [&](auto o) -> char {
                           lruc.insert(key1, value);
                           // lruc.insert(key2, value);
                           return o;
                           // stage 3, erase keys.
                         }) & tbb::make_filter<char, void>(tbb::filter::parallel, [&](auto o) {
                           // lruc.erase(key2);
                           lruc.erase(key1);

                           return o;
                           // stage 4, find erased keys.
                         }));

  IPLRUCache::ConstAccessor ca;
  bool result = lruc.find(ca, key1);
  ASSERT_FALSE(result) << "key found after erase";

  result = lruc.find(ca, key2);
  ASSERT_FALSE(result) << "key found after erase";
}
