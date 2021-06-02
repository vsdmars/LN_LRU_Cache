#include <gtest/gtest.h>
#include "lrucache.h"

/**
 * Test LRUCache
 *
 * 1. Basic insert/find/eviction test
 * 2. multi-thread access test
 * 3. Size/Stress test
 * 4. Test with IPAddress
 */

class LRUCacheTest : public ::testing::Test {
public:
  LRUCacheTest() {}

protected:
  void SetUp() override {}

  void TearDown() override {}
};

// create LRUCache instance
auto f = [](size_t size) { LRU };

// Demonstrate some basic assertions.
TEST(LRUCacheTest, Test1) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}
