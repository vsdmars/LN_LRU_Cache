// POSIX C Header
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

// CPP Header
#include <functional>
#include <string>

#include <tbb/concurrent_hash_map.h>

namespace AtsPluginUtils {
// Value
struct TimedEntityLookupInfo {
  long long int expiryTs;
  int denialInfoCode;
  int routingPrefixSize;
  bool requiresGoodBotUserAgent;
  TimedEntityLookupInfo(long long int ts, int f = 0, int pre_fix_size = 32, bool requires_good_user_agent = false)
      : expiryTs(ts),
        denialInfoCode(f),
        routingPrefixSize(pre_fix_size),
        requiresGoodBotUserAgent(requires_good_user_agent){};
};

// Key
struct IpAddress {
  union {
    struct sockaddr base;
    struct sockaddr_in v4;
    struct sockaddr_in6 v6;
  };

  IpAddress() { clear(); }

  // Implicit conversion from u_int32_t is allowed
  IpAddress(u_int32_t ip_v4) { set(ip_v4); }

  explicit IpAddress(const struct sockaddr* base) { set(base); }

  IpAddress(const IpAddress& rhs) { *this = rhs; }

  void clear() { memset(&v6, 0, sizeof(v6)); }

  void set(const struct sockaddr* base) {
    if (base->sa_family == AF_INET) {
      clear();
      memcpy(&v4, base, sizeof(v4));
    } else if (base->sa_family == AF_INET6) {
      memcpy(&v6, base, sizeof(v6));
    }
  }

  void set(u_int32_t ip_v4) {
    clear();
    base.sa_family = AF_INET;
    v4.sin_addr.s_addr = ip_v4;
  }

  IpAddress& operator=(const IpAddress& rhs) {
    if (this == &rhs) {
      return *this;
    }
    memcpy(&v6, &rhs.v6, sizeof(v6));
    return *this;
  }

  bool operator==(const IpAddress& rhs) const {
    if (this == &rhs) {
      return true;
    }
    if (base.sa_family != rhs.base.sa_family) {
      return false;
    }
    if (base.sa_family == AF_INET) {
      return (v4.sin_addr.s_addr == rhs.v4.sin_addr.s_addr);
    }
    return memcmp(&v6.sin6_addr.s6_addr, &rhs.v6.sin6_addr.s6_addr, 16) == 0;
  }

  bool operator!=(const IpAddress& rhs) const { return !(*this == rhs); }

  // Convert the IpAddress to readable string such as "1.2.3.4" or "2001:db8:8714::12".
  std::string toString() const {
    if (base.sa_family == AF_INET) {  // ipv4
      char str[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(v4.sin_addr), str, INET_ADDRSTRLEN);
      return std::string(str);
    } else if (base.sa_family == AF_INET6) {  // ipv6
      char str[INET6_ADDRSTRLEN];
      inet_ntop(AF_INET6, &(v6.sin6_addr), str, INET6_ADDRSTRLEN);
      return std::string(str);
    }
    return std::string();
  }
};

struct IpAddressHasher {
  size_t operator()(const IpAddress& ip) const {
    if (ip.base.sa_family == AF_INET) {
      return static_cast<size_t>(ip.v4.sin_addr.s_addr);
    }
    const size_t* ipHalves = reinterpret_cast<const size_t*>(&(ip.v6.sin6_addr.s6_addr));
    return ipHalves[0] ^ ipHalves[1];
  }
};

auto hash_ip_v4 = [](auto addr) {

};

auto hash_ip_v6 = [](auto addr) {

};

}  // namespace AtsPluginUtils

// https://spec.oneapi.com/versions/latest/elements/oneTBB/source/named_requirements/containers/hash_compare.html
namespace tbb {

using IpAddress = AtsPluginUtils::IpAddress;

template <>
class tbb_hash_compare<IpAddress> {
  static std::size_t hash(const IpAddress& k) {
    if (k.base.sa_family == AF_INET) {
      return std::hash<decltype(k.v4.sin_addr.s_addr)>{}();
    }
  }
  static bool equal(const IpAddress& k1, const IpAddress& k2) {}
};  // class tbb_hash_compare

}  // namespace tbb
