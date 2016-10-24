#pragma once
#include <initializer_list>
#include <vector>
#include <string>

#include "score/io/SocketAddr.h"
#include "score/io/Scstring.h"
#include "score/Try.h"

namespace score { namespace memcached {


class MemcachedConfig {
 public:
  using addr_t = score::io::SocketAddr;
  using string_t = score::io::Scstring;
 protected:
  std::vector<addr_t> serverHosts_;
 public:
  using server_init_list = std::initializer_list<addr_t>;
  MemcachedConfig();
  MemcachedConfig(server_init_list&& servers);

  template<typename TCollection>
  MemcachedConfig(const TCollection &servers) {
    addServers(servers);
  }

  void addServers(server_init_list&& servers);

  template<typename TCollection>
  void addServers(const TCollection &servers) {
    for (const addr_t& sock: servers) {
      serverHosts_.push_back(sock);
    }
  }

  bool hasAnyServers() const;
  score::Try<string_t> toConfigString();
};

namespace detail {
score::Try<MemcachedConfig::string_t> validateMemcachedConfigStr(
  const MemcachedConfig::string_t&
);
}

}} // fredis::memcached

