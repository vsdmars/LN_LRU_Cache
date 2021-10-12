#pragma once

#include <ats_type.h>

#include <boost/functional/hash.hpp>
#include <cstdint>
#include <functional>

namespace {
//  twang_mix64
//
//  Thomas Wang 64 bit mix hash function.
//  https://github.com/facebook/folly/blob/99fbca1df19fdd21f1b831cad6f50ece94573675/folly/hash/Hash.h#L87
constexpr uint64_t twang_mix64(uint64_t key) noexcept {
  key = (~key) + (key << 21);  // key *= (1 << 21) - 1; key -= 1;
  key = key ^ (key >> 24);
  key = key + (key << 3) + (key << 8);  // key *= 1 + (1 << 3) + (1 << 8)
  key = key ^ (key >> 14);
  key = key + (key << 2) + (key << 4);  // key *= 1 + (1 << 2) + (1 << 4)
  key = key ^ (key >> 28);
  key = key + (key << 31);  // key *= 1 + (1 << 31)
  return key;
}
}  // namespace

namespace std {
template <>
struct hash<AtsPluginUtils::IpAddress> {
  std::size_t operator()(AtsPluginUtils::IpAddress const& ip) const noexcept {
    size_t seed = twang_mix64(ip.base.sa_family);

    switch (ip.base.sa_family) {
      case AF_INET:
        boost::hash_combine(seed, twang_mix64(ip.v4.sin_addr.s_addr));
        break;
      case AF_INET6: {
        // The IPv6 address is 16 bytes long.
        // Combine it in blocks of sizeof(size_t) bytes each.
        static_assert(sizeof(struct in6_addr) % sizeof(size_t) == 0);

        const size_t* p = reinterpret_cast<const size_t*>(ip.v6.sin6_addr.s6_addr);
        constexpr auto in6_addr_size = sizeof(struct in6_addr);

        for (auto amtHashed = 0UL; amtHashed < in6_addr_size; amtHashed += sizeof(*p), ++p) {
          boost::hash_combine(seed, twang_mix64(*p));
        }
      }
    }

    return seed;
  }
};

template <>
struct equal_to<AtsPluginUtils::IpAddress> {
  constexpr bool operator()(const AtsPluginUtils::IpAddress& lhs, const AtsPluginUtils::IpAddress& rhs) const {
    return lhs == rhs;
  }
};

}  // namespace std
