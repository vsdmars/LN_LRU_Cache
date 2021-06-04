#pragma once

// POSIX C header
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

// CPP header
#include <string>

namespace AtsPluginUtils {

// Value, code from traffic team
struct TimedEntityLookupInfo {
  long long int expiryTs;
  int denialInfoCode;
  int routingPrefixSize;
  bool requiresGoodBotUserAgent;

  TimedEntityLookupInfo() = default;
  TimedEntityLookupInfo(long long int ts, int f = 0, int pre_fix_size = 32, bool requires_good_user_agent = false)
      : expiryTs(ts),
        denialInfoCode(f),
        routingPrefixSize(pre_fix_size),
        requiresGoodBotUserAgent(requires_good_user_agent){};
};

// Key, code from traffic team
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
}  // namespace AtsPluginUtils
