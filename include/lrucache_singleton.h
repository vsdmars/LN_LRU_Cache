/**
 * LRUcacheSingleton.h is the interface for plugins to use singleton LRUCache instance
 * in the same ATS process.
 *
 * plugins have to include this header and call the init function to initialize the cache
 * and use get function to retrieve the single instance of that cache.
 *
 * If the cache key is not language build-in types, caller has to implement a fully specialized
 * type template tbb::tbb_hash_compare in lrucache_tbb.h header.
 *
 * Caller can fully specialize non-type template AtsPluginUtils::CacheValue with
 * extended enum AtsPluginUtils::CACHE_VALUE_TYPE as non-type value to support different
 * cache value type.
 *
 * Currently support singleton cache key type:
 * AtsPluginUtils::IpAddress
 *
 * Currently support singleton cache value type:
 * AtsPluginUtils::CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>
 *
 * For usage examples, please refer to LRUcacheSingletonTest.cc
 *
 */
#pragma once

#include <lrucache_tbb.h>
#include <scale-lrucache.h>

namespace AtsPluginUtils {
inline namespace lrucache_v1 {

/**
 * CACHE_VALUE_TYPE enum is used as non-type template CacheValue parameter.
 *
 */
enum class CACHE_VALUE_TYPE {
  // TimedEntityLookupInfo: aggregate type contains IP state information.
  TIME_ENTITY_LOOKUP_INFO,
};

/**
 * CacheValue is a non-type template type with CACHE_VALUE_TYPE as parameter.
 * It is used as the value type for the LRUCache.
 *
 */
template <CACHE_VALUE_TYPE>
struct CacheValue;

template <>
struct CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO> {
  int64_t expiryTs;
  int denialInfoCode;

  CacheValue() = default;
  CacheValue(int64_t ts, int infoCode = 0) : expiryTs(ts), denialInfoCode(infoCode) {}
};

/**
 * IPTimeEntityCache type is created to support abuse-plugin/sentinel-plugin use case
 * which requires a singleton LRUCache across those 2 plug-ins with cache of type:
 *
 * key: AtsPluginUtils::IpAddress
 * value: AtsPluginUtils::CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>
 *
 */
using IPTimeEntityCache = LRUC::ScalableLRUCache<IpAddress, CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>>;

}  // namespace lrucache_v1
}  // namespace AtsPluginUtils

/**
 * init_ip_time_entity_cache initializes the singleton instance of LRUCache with type:
 * key: AtsPluginUtils::IpAddress
 * value: AtsPluginUtils::CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>
 *
 * Beware the initialization runs only once; first call wins.
 * Make sure the capacity and the sharded count hold the same values across the
 * plug-ins using the single instance cache.
 *
 * capacity: cache capacity
 * shardCnt: cache sharded count
 *
 */
void init_ip_time_entity_cache(size_t capacity, size_t shardCnt);

/**
 * get_ip_time_entity_cache returns single instance of the cache across plug-ins.
 * init_ip_time_entity_cache has to be called first before calling get_ip_time_entity_cache.
 *
 * Caller holds responsible for initializing the cache before calling this API.
 *
 */
AtsPluginUtils::IPTimeEntityCache& get_ip_time_entity_cache();
