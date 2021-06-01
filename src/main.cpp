// #include "lru_cache_cfg.h"
#include <iostream>
#include <vector>
#include <tbb/concurrent_hash_map.h>
#include "lrucache.h"
#include "scale-lrucache.h"

template <typename... T>
constexpr void unused(T... t) {
  ((void(t)), ...);
}

template <typename Key, typename Value, typename... T>
using HashMap = tbb::concurrent_hash_map<Key, Value, T...>;

template <typename Key, typename Value>
using HashMapAccessor = typename HashMap<Key, Value>::accessor;

template <typename Key, typename Value>
using HashMapConstAccessor = typename HashMap<Key, Value>::const_accessor;

template <typename Key, typename Value>
using HashMapValuePair = typename HashMap<Key, Value>::value_type;

template <typename T, typename V>
T getKeyType(HashMap<T, V>);
template <typename T, typename V>
V getValueType(HashMap<T, V>);

auto main(int argc, char* argv[]) -> int {
  unused(argc, argv);

  // if (argc < 2) {
  // // report version
  // std::cout << argv[0] << " Version " << LRU_CACHE_MAJOR_VERSION << "." <<
  // LRU_CACHE_MINOR_VERSION
  // << std::endl;
  // std::cout << "Usage: " << argv[0] << " number" << std::endl;
  // return 1;
  // }

  HashMap<int, int> hmap{4};
  HashMapAccessor<decltype(getKeyType(hmap)), decltype(getValueType(hmap))> accessor;
  HashMapValuePair<decltype(getKeyType(hmap)), decltype(getValueType(hmap))> value{42, 42};

  hmap.insert(accessor, value);

  if (hmap.find(accessor, 42)) {
    std::cout << accessor->second << std::endl;
  }

  // test LRUCache
  LRUC::LRUCache<int, int> lruc{3};
  lruc.insert(1, 11);
  lruc.insert(2, 22);
  lruc.insert(3, 33);
  lruc.insert(4, 44);

  LRUC::LRUCache<int, int>::ConstAccessor ca;
  if (!lruc.find(ca, 1)) {
    std::cout << "lruc key 1 not found" << std::endl;
  }

  lruc.find(ca, 3);
  std::cout << *ca << std::endl;

  // ScalableLRUCache
  LRUC::ScalableLRUCache<int, int> slruc{7};
  slruc.insert(1, 11);
  slruc.insert(2, 22);
  slruc.insert(3, 33);
  slruc.insert(4, 44);
  slruc.insert(5, 55);
  slruc.insert(6, 66);
  slruc.insert(7, 77);
  slruc.insert(8, 88);

  LRUC::ScalableLRUCache<int, int>::ConstAccessor sca;
  if (!slruc.find(sca, 1)) {
    std::cout << "slruc key 1 not found" << std::endl;
  }
  slruc.find(sca, 3);
  std::cout << *sca << std::endl;

  std::cout << "end"
            << "\n";
}
