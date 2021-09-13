#include "lrucache_api.h"

// posix header
#include <arpa/inet.h>
#include <sys/socket.h>

#include <thread>
#include <gtest/gtest.h>

using std::thread;
using namespace AtsPluginUtils;

namespace AtsPluginUtils {

constexpr int TEST_TYPE = 42;

template<>
struct CacheValue<static_cast<CACHE_VALUE_TYPE>(TEST_TYPE)> {
  int value;
  CacheValue() = default;
};

using testCache = LRUC::ScalableLRUCache<int, CacheValue<static_cast<CACHE_VALUE_TYPE>(TEST_TYPE)>>;
} // namespace AtsPluginUtils

/**
 * create_IpAddress is a callable object taking IPv4 address as const char* and returns IpAddress
 *
 */
auto create_IpAddress = [](auto ipv4) -> IpAddress {
  sockaddr_in socket;
  socket.sin_family = AF_INET;
  socket.sin_port = 42;

  if (inet_pton(AF_INET, ipv4, &socket.sin_addr) == 1) {
    IpAddress ipa{(sockaddr*)& socket};
    return ipa;
  }

  return IpAddress{};
};

/**
 * singletonTest tests the cache instances returned from different DSO
 * point to the same memory address.
 * Thus, the inserted key/value from one DSO can be retrieved from another DSO.
 *
 */
TEST(LRUcacheSingletonTest, singletonTest) {
  thread t1([] {init_cache1();});
  thread t2([] {init_cache2();});
  t1.join();
  t2.join();

  const char* ip1 = "192.168.1.1";
  const char* ip2 = "192.168.1.2";
  auto key1 = create_IpAddress(ip1);
  auto key2 = create_IpAddress(ip2);

  constexpr auto ts1 = 1'629'834'401;
  constexpr auto ts2 = 1'629'834'402;
  AtsPluginUtils::CacheValue<AtsPluginUtils::CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO> value1;
  AtsPluginUtils::CacheValue<AtsPluginUtils::CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO> value2;
  value1.expiryTs = ts1;
  value2.expiryTs = ts2;

  cache1_insert(key1, value1);
  auto retrieved_value1 = cache2_find(key1);
  EXPECT_EQ(retrieved_value1.expiryTs, ts1);

  cache2_insert(key2, value2);
  auto retrieved_value2 = cache1_find(key2);
  EXPECT_EQ(retrieved_value2.expiryTs, ts2);

  auto& cache_1 = get_cache1(); // retrieve cache from liblrucache1.so
  auto& cache_2 = get_cache2(); // retrieve cache from liblrucache2.so

  EXPECT_EQ(cache_1.capacity(), magic_cache_size);
  EXPECT_EQ(cache_1.capacity(), cache_2.capacity());
  ASSERT_EQ(&cache_1, &cache_2);
}

/**
 * differentCacheValueTypeTest tests the cache insert key with different value type.
 *
 */
TEST(LRUcacheSingletonTest, differentCacheValueTypeTest) {
  auto cache = AtsPluginUtils::testCache{42, 4};

  constexpr auto val = 1'629'834'401;

  AtsPluginUtils::CacheValue<static_cast<AtsPluginUtils::CACHE_VALUE_TYPE>(AtsPluginUtils::TEST_TYPE)> value;
  value.value = val;

  cache.insert(42, value);

  AtsPluginUtils::testCache::ConstAccessor ca;
  EXPECT_EQ(cache.find(ca, 42), true);
  EXPECT_EQ(ca->value, val);
  ASSERT_EQ(cache.size(), 1);

  EXPECT_EQ(cache.erase(42), 1);
  ASSERT_EQ(cache.size(), 0);
}
