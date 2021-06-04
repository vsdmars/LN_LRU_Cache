#pragma once

#include <gtest/gtest.h>

// posix header
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// CPP header
#include <iostream>
#include <random>
#include <sstream>
#include <thread>
#include <unordered_set>
#include <utility>

// intel TBB header
#include <tbb/parallel_for_each.h>
#include "tbb/pipeline.h"

#include "ats_tbb.h"
#include "ats_type.h"
#include "lrucache.h"
#include "scale-lrucache.h"

using namespace AtsPluginUtils;
using IPLRUCache = LRUC::LRUCache<IpAddress, TimedEntityLookupInfo>;
using SCALE_IPLRUCache = LRUC::ScalableLRUCache<IpAddress, TimedEntityLookupInfo>;

namespace {
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

template <>
void containerInsert(SCALE_IPLRUCache& lruc, int b, int c, int d, int expiryTS) {
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
}  // namespace
