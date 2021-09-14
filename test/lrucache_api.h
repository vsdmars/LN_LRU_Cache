/**
 * lrucache_api.h define the interface for testing lrucache singleton share between DSOs
 */
#pragma once

#include "LRUcacheSingleton.h"

using namespace AtsPluginUtils;

using Value = CacheValue<CACHE_VALUE_TYPE::TIME_ENTITY_LOOKUP_INFO>;
constexpr auto magic_cache_size = 42;


void init_cache1();
void cache1_insert(IpAddress key, Value value);
Value cache1_find(IpAddress key);
IPTimeEntityCache& get_cache1();

void init_cache2();
void cache2_insert(IpAddress key, Value value);
Value cache2_find(IpAddress key);
IPTimeEntityCache& get_cache2();
