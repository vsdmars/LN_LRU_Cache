#include "LRUcacheSingleton.h"

using namespace AtsPluginUtils;
using Value = CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>;
constexpr auto magic_cache_size = 42;

void init_cache1() {
  init_ip_time_entity_cache(magic_cache_size, 4);
}

IPTimeEntityCache& get_cache1() {
  return get_ip_time_entity_cache();
}

void cache1_insert(IpAddress key, Value value) {
  auto& cache = get_ip_time_entity_cache();
  cache.insert(key, value);
}

Value cache1_find(IpAddress key) {
  auto& cache = get_ip_time_entity_cache();
  AtsPluginUtils::IPTimeEntityCache::ConstAccessor ca;
  cache.find(ca, key);

  return *ca;
}
