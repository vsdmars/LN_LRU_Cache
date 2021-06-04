#include <gtest/gtest.h>

// posix header
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// CPP header
#include <iostream>
#include <random>
#include <sstream>
#include <unordered_set>
#include <utility>

// intel TBB header
#include <tbb/parallel_for_each.h>

#include "ats_type.h"
#include "lrucache.h"

/**
 * Test LRUCache
 *
 * Key type: IpAddress
 * Value type: TimedEntityLookupInfo
 */

using namespace AtsPluginUtils;
using IPLRUCache = LRUC::LRUCache<IpAddress, TimedEntityLookupInfo>;

// AtsPluginUtils::IpAddress generator
auto create_IpAddress = [](std::string ipv4) -> IpAddress {
  sockaddr_in socket;
  socket.sin_family = AF_INET;
  socket.sin_port = 42;

  if (inet_pton(AF_INET, ipv4.c_str(), &socket.sin_addr) == 1) {
    IpAddress ipa{reinterpret_cast<sockaddr*>(&socket)};
    return ipa;
  }

  return IpAddress{};
};

// AtsPluginUtils:: TimedEntityLookupInfo generator
auto create_teli = [](auto ts) -> TimedEntityLookupInfo { return TimedEntityLookupInfo{ts}; };

// int generator [from, to)
auto generator(int from, int to) {
  return [=] {
    decltype(from) i = from;
    return [=]() mutable { return i < to ? i++ : -1; };
  }();
}

// generate class C IPv4 address start with 192 in string type
std::string getIPv4(int b, int c, int d) {
  std::stringstream ipv4;

  ipv4 << 192 << '.' << b << "." << c << "." << d;
  return ipv4.str();
}

template <typename T>
void containerInsert(T&& t, int b, int c, int d, int expiryTS) {
  std::stringstream ipv4;
  ipv4 << "192." << b << "." << c << "." << d;
  t.insert(ipv4.str());
}

template <>
void containerInsert(IPLRUCache& lruc, int b, int c, int d, int expiryTS) {
  lruc.insert(create_IpAddress(getIPv4(b, c, d)), create_teli(expiryTS));
}

template <typename T>
void ipJob(T&& t, int bfrom, int bto, int cfrom, int cto, int dfrom, int dto, int expiryTS = 0) {
  auto B = generator(bfrom, bto);

  while (true) {
    if (auto b = B(); b != -1) {
      auto C = generator(cfrom, cto);
      while (true) {
        if (auto c = C(); c != -1) {
          auto D = generator(dfrom, dto);
          while (true) {
            if (auto d = D(); d != -1) {
              containerInsert(std::forward<T>(t), b, c, d, expiryTS);
            } else {
              break;
            }
          }
        } else {
          break;
        }
      }
    } else {
      break;
    }
  }
}

class LRUCache2MTest : public ::testing::Test {
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

  IPLRUCache lruc{LRUC_SIZE};

protected:
  void SetUp() override { ipJob(lruc, bfrom, bto, cfrom, cto, dfrom, dto, EXPIRYTS); }
  void TearDown() override {}
};

/**
 * Single thread access LRU cache test
 */
TEST_F(LRUCache2MTest, TestSingleThread) {
  constexpr const char* EVICTED_IP = "192.0.0.0";
  constexpr const char* NOT_EVICTED_IP = "192.0.0.1";

  ASSERT_EQ(LRUC_SIZE, lruc.size()) << "lruc.size() result not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "lruc.capacity() result not match";

  // random generator
  std::uniform_int_distribution<> rangeB{0, 28};
  std::uniform_int_distribution<> rangeCD{1, 254};
  std::uniform_int_distribution<> rangeFalseB{29, 254};

  IPLRUCache::ConstAccessor ca;
  std::stringstream randomFalseIPv4;
  randomFalseIPv4 << "192." << rangeFalseB(gen) << "." << rangeCD(gen) << "." << rangeCD(gen);
  EXPECT_FALSE(lruc.find(ca, create_IpAddress("192.192.1.1")))
      << "IP [" << randomFalseIPv4.str() << "] shouldn't be found in lru cache";

  std::stringstream randomIPv4;
  randomIPv4 << "192." << rangeB(gen) << "." << rangeCD(gen) << "." << rangeCD(gen);
  EXPECT_TRUE(lruc.find(ca, create_IpAddress(randomIPv4.str())))
      << "IP [" << randomIPv4.str() << "] can't be found in lru cache";

  EXPECT_EQ(EXPIRYTS, (*ca).expiryTs);

  lruc.insert(create_IpAddress(randomFalseIPv4.str()), create_teli(EXPIRYTS));
  EXPECT_FALSE(lruc.find(ca, create_IpAddress(EVICTED_IP)));
  EXPECT_TRUE(lruc.find(ca, create_IpAddress(NOT_EVICTED_IP)));

  lruc.clear();
  EXPECT_FALSE(lruc.find(ca, create_IpAddress(NOT_EVICTED_IP))) << "LRU cache cleared but IP key still can be found";
  EXPECT_EQ(0, lruc.size()) << "LRU cache cleared but size still show not 0";

  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "lruc.capacity() result not match";
}

/**
 * multi-threads access LRU cache test.
 */
TEST_F(LRUCache2MTest, TestMultiThread_1) {
  constexpr const char* EVICTED_IP = "192.0.0.0";

  ASSERT_EQ(LRUC_SIZE, lruc.size()) << "lruc.size() result not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "lruc.capacity() result not match";

  // random generator
  std::uniform_int_distribution<> rangeB{0, 28};
  std::uniform_int_distribution<> rangeCD{1, 254};
  std::uniform_int_distribution<> rangeParallelB{29, 254};

  constexpr int ipCnt = 192;
  std::unordered_set<std::string> randomIPs{ipCnt};

  auto i = 0;
  while (i < ipCnt) {
    std::stringstream randomIPv4;
    randomIPv4 << "192." << rangeParallelB(gen) << "." << rangeCD(gen) << "." << rangeCD(gen);
    randomIPs.insert(randomIPv4.str());

    ++i;
  }

  tbb::parallel_for_each(randomIPs, [this](auto item) {
    IPLRUCache::ConstAccessor ca;
    // insert IP concurrently
    lruc.insert(create_IpAddress(item), create_teli(EXPIRYTS));
    EXPECT_TRUE(lruc.find(ca, create_IpAddress(item)));
  });

  IPLRUCache::ConstAccessor ca;
  EXPECT_FALSE(lruc.find(ca, create_IpAddress(EVICTED_IP)));

  ASSERT_EQ(LRUC_SIZE, lruc.size()) << "lruc.size() result not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "lruc.capacity() result not match";
}

/**
 * multi-threads access LRU cache test.
 *
 * Flush out LRUC_SIZE (1'885'725) of IPs with new ones
 */
TEST_F(LRUCache2MTest, TestMultiThread_2) {
  ASSERT_EQ(LRUC_SIZE, lruc.size()) << "lruc.size() result not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "lruc.capacity() result not match";

  constexpr int rbfrom = 29;
  constexpr int rbto = 58;
  std::unordered_set<std::string> flushOutIPs{LRUC_SIZE};

  ipJob(flushOutIPs, rbfrom, rbto, cfrom, cto, dfrom, dto, EXPIRYTS);

  tbb::parallel_for_each(flushOutIPs, [this](auto item) {
    // insert IP concurrently
    lruc.insert(create_IpAddress(item), create_teli(EXPIRYTS));
  });

  ASSERT_EQ(LRUC_SIZE, lruc.size()) << "lruc.size() result not match";
  ASSERT_EQ(LRUC_SIZE, lruc.capacity()) << "lruc.capacity() result not match";

  {
    // random generator
    std::uniform_int_distribution<> rangeB{0, 28};
    std::uniform_int_distribution<> rangeCD{0, 254};

    // prepare flushed out random IP testing
    constexpr int ipCnt = 192;
    std::unordered_set<std::string> randomIPs{ipCnt};

    auto i = 0;
    while (i < ipCnt) {
      std::stringstream randomIPv4;
      randomIPv4 << "192." << rangeB(gen) << "." << rangeCD(gen) << "." << rangeCD(gen);
      randomIPs.insert(randomIPv4.str());

      ++i;
    }

    tbb::parallel_for_each(randomIPs, [this](auto item) {
      IPLRUCache::ConstAccessor ca;
      // insert IP concurrently
      EXPECT_FALSE(lruc.find(ca, create_IpAddress(item)));
    });
  }
  {
    // random generator
    std::uniform_int_distribution<> rangeB{29, 57};
    std::uniform_int_distribution<> rangeCD{0, 254};

    // prepare flushed out random IP testing
    constexpr int ipCnt = 192;
    std::unordered_set<std::string> randomIPs{ipCnt};

    auto i = 0;
    while (i < ipCnt) {
      std::stringstream randomIPv4;
      randomIPv4 << "192." << rangeB(gen) << "." << rangeCD(gen) << "." << rangeCD(gen);
      randomIPs.insert(randomIPv4.str());

      ++i;
    }

    tbb::parallel_for_each(randomIPs, [this](auto item) {
      IPLRUCache::ConstAccessor ca;
      // insert IP concurrently
      EXPECT_FALSE(lruc.find(ca, create_IpAddress(item)));
    });
  }
}
