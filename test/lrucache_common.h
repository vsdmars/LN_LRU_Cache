/**
 * Common header for LRUCache unit test
 *
 */
#pragma once

#include <gtest/gtest.h>

// intel TBB header
#include <tbb/parallel_for_each.h>
#include <tbb/pipeline.h>

#include <lrucache_singleton.h>

// posix header
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// CPP header
#include <array>
#include <iostream>
#include <random>
#include <sstream>
#include <thread>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace AtsPluginUtils;

/**
 * IPLRUCache is LRUC::LRUCache cache with
 * key: AtsPluginUtils::IpAddress
 * value: AtsPluginUtils::CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>
 *
 */
using IPLRUCache = LRUC::LRUCache<IpAddress, CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>>;

/**
 * SCALE_IPLRUCache is LRUC::ScalableLRUCache cache with
 * key: AtsPluginUtils::IpAddress
 * value: AtsPluginUtils::CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>
 *
 */
using SCALE_IPLRUCache = LRUC::ScalableLRUCache<IpAddress, CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>>;

namespace {

/**
 * create_IpAddress is a callable object, taking ipv4 string (e.g '192.168.1.1')
 * and returns AtsPluginUtils::IpAddress instance.
 *
 */
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

/**
 * create_cache_value is a callable object, taking timestamp (e.g 1'222'333'444)
 * and returns CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO> instance
 *
 */
auto create_cache_value = [](auto ts) -> CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO> {
  return CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>{ts};
};

/**
 * generator generates number from 'from' to 'to' exclusively and returns a callable object,
 * which each call to the object returns a number sequentially.
 *
 */
auto generator(int from, int to) {
  return [=] {
    decltype(from) i = from;
    return [=]() mutable { return i < to ? i++ : -1; };
  }();
}

/**
 * getIPv4 generates IPv4 class C address as string (e.g '192.b.c.d')
 *
 */
std::string getIPv4(int b, int c, int d) {
  std::stringstream ipv4;

  ipv4 << 192 << '.' << b << "." << c << "." << d;
  return ipv4.str();
}

/**
 * containerInsert inserts IPv4 class C address string into associate container t.
 *
 */
template <typename T>
void containerInsert(T&& t, int b, int c, int d, int expiryTS) {
  std::stringstream ipv4;
  ipv4 << "192." << b << "." << c << "." << d;
  t.insert(ipv4.str());
}

/**
 * containerInsert inserts IPv4 class C address string into vector container with value
 * CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>
 *
 */
template <typename T, typename... U>
void containerInsert(std::vector<T, U...>& t, int b, int c, int d, int expiryTS) {
  std::stringstream ipv4;
  ipv4 << "192." << b << "." << c << "." << d;

  t.emplace_back(create_IpAddress(ipv4.str()), create_cache_value(expiryTS));
}

/**
 * containerInsert inserts IPv4 class C address into LRUC::LRUCache with value
 * CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>
 *
 */
template <>
void containerInsert(IPLRUCache& lruc, int b, int c, int d, int expiryTS) {
  lruc.insert(create_IpAddress(getIPv4(b, c, d)), create_cache_value(expiryTS));
}

/**
 * containerInsert inserts IPv4 class C address into LRUC::ScalableLRUCache with value
 * CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>
 *
 */
template <>
void containerInsert(SCALE_IPLRUCache& lruc, int b, int c, int d, int expiryTS) {
  lruc.insert(create_IpAddress(getIPv4(b, c, d)), create_cache_value(expiryTS));
}

/**
 * ipJob fills the container/cache t with ranged IPv4 class address (e.g '192.b.c.d')
 * with value CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>
 *
 */
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

}  // namespace
