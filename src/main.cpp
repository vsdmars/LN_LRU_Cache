// posix header
#include <arpa/inet.h>
#include <sys/socket.h>

// cpp header
#include <iostream>
#include <vector>
#include <tbb/concurrent_hash_map.h>

#include "lrucache.h"
#include "scale-lrucache.h"
#include "ats_tbb.h"
#include "ats_type.h"

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

using namespace AtsPluginUtils;
auto create_IpAddress = [](auto ipv4) -> IpAddress {
  sockaddr_in socket;
  socket.sin_family = AF_INET;
  socket.sin_port = 42;

  if (inet_pton(AF_INET, ipv4, &socket.sin_addr) == 1) {
    IpAddress ipa{(sockaddr*)&socket};
    return ipa;
  }

  return IpAddress{};
};

// create TimedEntityLookupInfo
auto create_teli = [](auto ts) -> TimedEntityLookupInfo { return TimedEntityLookupInfo{ts}; };

auto main(int argc, char* argv[]) -> int {
  unused(argc, argv);

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
  std::cout << "found val: " << *ca << std::endl << std::endl;

  // test ScalableLRUCache
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
    std::cout << "slruc key 1 not found\n" << std::flush;
  } else {
    std::cout << "slruc key 1 found\n" << std::flush;
  }

  slruc.find(sca, 8);
  if (!sca.empty()) {
    std::cout << "found val: " << *sca << '\n' << std::flush;
  }

  std::cout << "------------\n" << std::flush;
  // test LRUCache IpAddress/TimedEntityLookupInfo
  LRUC::LRUCache<IpAddress, TimedEntityLookupInfo> lruc_ip_addr{3};

  lruc_ip_addr.insert(create_IpAddress("192.168.1.1"), create_teli(41));
  lruc_ip_addr.insert(create_IpAddress("192.168.1.2"), create_teli(42));
  lruc_ip_addr.insert(create_IpAddress("192.168.1.3"), create_teli(43));
  lruc_ip_addr.insert(create_IpAddress("193.168.1.4"), create_teli(44));

  LRUC::LRUCache<IpAddress, TimedEntityLookupInfo>::ConstAccessor ca2;
  if (!lruc_ip_addr.find(ca2, create_IpAddress("192.168.1.1"))) {
    std::cout << "lruc_ip_addr key 192.168.1.1 not found" << std::endl;
  }

  if (!lruc_ip_addr.find(ca2, create_IpAddress("192.168.1.3"))) {
    std::cout << "lruc_ip_addr key 192.168.1.3 not found" << std::endl;
  } else {
    std::cout << "found 192.168.1.3 expiryTs val: " << (*ca2).expiryTs << std::endl << std::endl;
  }
}
