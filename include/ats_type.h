
struct TimedEntityLookupInfo {
  int64_t expiryTs;
  int denialInfoCode;
  int routingPrefixSize;
  bool requiresGoodBotUserAgent;
  TimedEntityLookupInfo(int64_t ts, int f = 0, int prefixSize = 32, bool requiresGoodUserAgent = false) :
    expiryTs(ts), denialInfoCode(f), routingPrefixSize(prefixSize), requiresGoodBotUserAgent(requiresGoodUserAgent) { };
};

struct IpAddress {
  union {
    struct sockaddr base;
    struct sockaddr_in v4;
    struct sockaddr_in6 v6;
  };

  IpAddress() {
    clear();
  }

  // Implicit conversion from u_int32_t is allowed
  IpAddress(u_int32_t v4Ip) {
    set(v4Ip);
  }

  explicit IpAddress(const struct sockaddr* base) {
    set(base);
  }

  IpAddress(const IpAddress& rhs) {
    *this = rhs;
  }

  void clear() {
    memset(&v6, 0, sizeof(v6));
  }

  void set(const struct sockaddr* base) {
    if (base->sa_family == AF_INET) {
      clear();
      memcpy(&v4, base, sizeof(v4));
    } else if (base->sa_family == AF_INET6) {
      memcpy(&v6, base, sizeof(v6));
    }
  }

  void set(u_int32_t v4Ip) {
    clear();
    base.sa_family = AF_INET;
    v4.sin_addr.s_addr = v4Ip;
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

  bool operator!=(const IpAddress& rhs) const {
    return !(*this == rhs);
  }

  // Convert the IpAddress to readable string such as "1.2.3.4" or "2001:db8:8714::12".
  std::string toString() const {
    if (base.sa_family == AF_INET) { // ipv4
      char str[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(v4.sin_addr), str, INET_ADDRSTRLEN);
      return std::string(str);
    } else if (base.sa_family == AF_INET6) { // ipv6
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
