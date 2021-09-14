/**
 * Defines declared functions in LRUcacheSingleton.h
 *
 */
#include <lrucache_singleton.h>

#include <mutex>

namespace {
std::once_flag init_ip_time_entity_cache_flag;
size_t ip_time_entity_cache_capacity;
size_t ip_time_entity_cache_shardCount;
}  // namespace

void init_ip_time_entity_cache(size_t capacity, size_t shardCnt) {
  std::call_once(init_ip_time_entity_cache_flag, [=] {
    ip_time_entity_cache_capacity = capacity;
    ip_time_entity_cache_shardCount = shardCnt;
  });
}

AtsPluginUtils::IPTimeEntityCache& get_ip_time_entity_cache() {
  static AtsPluginUtils::IPTimeEntityCache cache{ip_time_entity_cache_capacity, ip_time_entity_cache_shardCount};

  return cache;
}
