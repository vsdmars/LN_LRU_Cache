#include "common_test.h"

/**
 * Test ScalableLRUCache
 *
 * Key type: IpAddress
 * Value type: TimedEntityLookupInfo
 */

/**
 * Init. ScalableLRUCache with 2M entries
 */
class ScaleLRUCache2MTest : public ::testing::Test {
protected:
  constexpr static int LRUC_SIZE = 1'885'725;
  constexpr static int EXPIRYTS = 42;

  constexpr static int bfrom{0};
  constexpr static int bto{29};
  constexpr static int cfrom{0};
  constexpr static int cto{255};
  constexpr static int dfrom{0};
  constexpr static int dto{255};

  std::random_device rd{};
  std::mt19937 gen{rd()};

  SCALE_IPLRUCache lruc{LRUC_SIZE};

protected:
  void SetUp() override { ipJob(lruc, bfrom, bto, cfrom, cto, dfrom, dto, EXPIRYTS); }
  void TearDown() override {}
};

/**
 * Single thread access LRU cache test.
 */
TEST_F(ScaleLRUCache2MTest, TestSingleThread) {
  std::cout << "HW Core count: [" << std::thread::hardware_concurrency() << "]\n" << std::flush;

  // Use greater or equal assertion due to even though twang_mix64 hash is uniformed there are chances skew a bit
  // thus some shard bucket are full(and evicted due to hits the capacity) and some are little less thus
  // sum up the total size be less or equal to inserted IP counts.
  ASSERT_GE(LRUC_SIZE, lruc.size()) << "lruc.size() is greater than init. cache size!";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "lruc.capacity() result not match";

  for (int i = 0; i < lruc.shardCount(); i++) {
    std::cout << "Shard[" << i << "] size: [" << lruc.size(i) << "]\n" << std::flush;

    EXPECT_GE(LRUC_SIZE / lruc.shardCount(), lruc.size(i)) << "Shard[" << i << "] size is too large";
  }

  // random generator
  std::uniform_int_distribution<> rangeB{0, 28};
  std::uniform_int_distribution<> rangeCD{1, 254};
  std::uniform_int_distribution<> rangeFalseB{29, 254};

  SCALE_IPLRUCache::ConstAccessor ca;
  std::stringstream randomFalseIPv4;
  randomFalseIPv4 << "192." << rangeFalseB(gen) << "." << rangeCD(gen) << "." << rangeCD(gen);
  EXPECT_FALSE(lruc.find(ca, create_IpAddress(randomFalseIPv4.str())))
      << "IP [" << randomFalseIPv4.str() << "] shouldn't be found in lru cache";

  std::stringstream randomIPv4;
  randomIPv4 << "192.28." << rangeCD(gen) << "." << rangeCD(gen);
  EXPECT_TRUE(lruc.find(ca, create_IpAddress(randomIPv4.str())))
      << "IP [" << randomIPv4.str() << "] can't be found in lru cache";
  EXPECT_EQ(EXPIRYTS, (*ca).expiryTs);

  lruc.insert(create_IpAddress(randomFalseIPv4.str()), create_teli(EXPIRYTS));
  EXPECT_TRUE(lruc.find(ca, create_IpAddress(randomFalseIPv4.str().c_str())));

  lruc.clear();
  EXPECT_FALSE(lruc.find(ca, create_IpAddress(randomFalseIPv4.str().c_str())))
      << "LRU cache cleared but IP key still can be found";
  ASSERT_EQ(0, lruc.size()) << "LRU cache cleared but size still show not 0";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "lruc.capacity() result not match";
}

/**
 * multi-threads access LRU cache test.
 *
 * 1. Flush out LRUC_SIZE (1'885'725) IPs and filled with new ones.
 * 2. Read IPs concurrently during the flush out.
 */
TEST_F(ScaleLRUCache2MTest, TestMultiThread_1) {
  std::cout << "HW Core count: [" << std::thread::hardware_concurrency() << "]\n" << std::flush;

  ASSERT_GE(LRUC_SIZE, lruc.size()) << "lruc.size() is greater than init. cache size!";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "lruc.capacity() result not match";

  constexpr int rbfrom = 29;
  constexpr int rbto = 58;
  std::unordered_set<std::string> flushOutIPs{LRUC_SIZE};

  // fill the container flushOutIPs with new IPs.
  ipJob(flushOutIPs, rbfrom, rbto, cfrom, cto, dfrom, dto, EXPIRYTS);

  // insert 2M IPs concurrently.
  tbb::parallel_for_each(flushOutIPs, [this](auto item) {
    // insert IP concurrently
    lruc.insert(create_IpAddress(item), create_teli(EXPIRYTS));
  });

  auto si = std::begin(flushOutIPs);
  auto ei = std::end(flushOutIPs);
  int ipCnt = 0;

  // processing pipeline
  // stage 1. pipe IPs in sequence.
  // stage 2. insert IPs into ScalableLRUCache in parallel.
  // stage 3. IPs lookup in ScalableLRUCache while insert continues to happen.
  // stage 4. update total IP insertion count.
  tbb::parallel_pipeline(
      std::thread::hardware_concurrency() * 2,
      tbb::make_filter<void, std::string>(tbb::filter::serial, [&](tbb::flow_control& fc) -> std::string {
        if (si != ei) {
          return *si++;
        } else {
          fc.stop();
          return "";
        }
      }) & tbb::make_filter<std::string, std::string>(tbb::filter::parallel, [this](auto ipv4) -> std::string {
        lruc.insert(create_IpAddress(ipv4), create_teli(EXPIRYTS));
        return ipv4;
      }) & tbb::make_filter<std::string, int>(tbb::filter::parallel, [this](auto ipv4) -> int {
        SCALE_IPLRUCache::ConstAccessor ca;
        bool result = lruc.find(ca, create_IpAddress(ipv4));

        EXPECT_TRUE(result) << "IP: [" << ipv4 << "] not found";

        return result;
      }) & tbb::make_filter<int, void>(tbb::filter::serial, [&ipCnt](auto cnt) { ipCnt += cnt; }));

  ASSERT_EQ(LRUC_SIZE, ipCnt) << "IP count not match";
  ASSERT_EQ(LRUC_SIZE, lruc.size()) << "lruc.size() result not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "lruc.capacity() result not match";
}