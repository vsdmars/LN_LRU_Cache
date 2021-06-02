size_t TSocketAddress::hash() const {
  size_t seed = folly::hash::twang_mix64(storage_.addr.sa_family);

  switch (storage_.addr.sa_family) {
    case AF_INET:
      boost::hash_combine(seed,
                          folly::hash::twang_mix64(storage_.ipv4.sin_port));
      boost::hash_combine(
          seed, folly::hash::twang_mix64(storage_.ipv4.sin_addr.s_addr));
      break;
    case AF_INET6: {
      boost::hash_combine(seed,
                          folly::hash::twang_mix64(storage_.ipv6.sin6_port));
      // The IPv6 address is 16 bytes long.
      // Combine it in blocks of sizeof(size_t) bytes each.
      BOOST_STATIC_ASSERT(sizeof(struct in6_addr) % sizeof(size_t) == 0);
      const size_t* p =
        reinterpret_cast<const size_t*>(storage_.ipv6.sin6_addr.s6_addr);
      for (int amtHashed = 0;
           amtHashed < sizeof(struct in6_addr);
           amtHashed += sizeof(*p), ++p) {
        boost::hash_combine(seed, folly::hash::twang_mix64(*p));
      }
      boost::hash_combine(
          seed, folly::hash::twang_mix64(storage_.ipv6.sin6_scope_id));
      break;
    }
    case AF_UNIX:
    {
      enum { kUnixPathMax = sizeof(storage_.un.addr->sun_path) };
      const char *path = storage_.un.addr->sun_path;
      size_t pathLength = storage_.un.pathLength();
      // TODO: this probably could be made more efficient
      for (unsigned int n = 0; n < pathLength; ++n) {
        boost::hash_combine(seed, folly::hash::twang_mix64(path[n]));
      }
      break;
    }
    case AF_UNSPEC:
    default:
      throw TTransportException(TTransportException::INTERNAL_ERROR,
                                "TSocketAddress: unsupported address family "
                                "for hashing");
  }

  return seed;
}
