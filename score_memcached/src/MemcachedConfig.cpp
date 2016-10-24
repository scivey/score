#include "score_memcached/MemcachedConfig.h"
#include "score_memcached/MemcachedError.h"
#include "score/ExceptionWrapper.h"
#include "score/Try.h"
#include "score/util/try_helpers.h"

#include <libmemcached/memcached.h>

namespace score { namespace memcached {

using string_t = typename MemcachedConfig::string_t;
using addr_t = typename MemcachedConfig::addr_t;

MemcachedConfig::MemcachedConfig(){}

MemcachedConfig::MemcachedConfig(MemcachedConfig::server_init_list&& servers)
  : serverHosts_(std::forward<MemcachedConfig::server_init_list>(servers)){}

void MemcachedConfig::addServers(MemcachedConfig::server_init_list&& servers) {
  for (auto&& server: servers) {
    serverHosts_.push_back(server);
  }
}

Try<string_t> MemcachedConfig::toConfigString() {
  std::ostringstream oss;
  size_t lastIdx = serverHosts_.size();
  if (lastIdx > 0) {
    lastIdx--;
  }
  size_t idx = 0;
  for (auto &server: serverHosts_) {
    oss << "--SERVER=" << server.getHost() << ":" << server.getPort();
    if (idx < lastIdx) {
      oss << " ";
    }
    idx++;
  }
  string_t confStr = oss.str();
  auto err = detail::validateMemcachedConfigStr(confStr);
  if (err.hasException()) {
    return err;
  }

  return Try<string_t>{confStr};
}

bool MemcachedConfig::hasAnyServers() const {
  return serverHosts_.size() > 0;
}

namespace detail {

Try<string_t> validateMemcachedConfigStr(const string_t& configStr) {
  char errBuff[512];
  auto rc = libmemcached_check_configuration(configStr.c_str(),
    configStr.size(), (char*) errBuff, sizeof(errBuff));
  if (memcached_failed(rc)) {
    std::ostringstream msg;
    msg << "Configuration Error: '" << errBuff << "'";
    return util::makeTryFailure<string_t, ConfigurationError>(msg.str());
  }
  return util::makeTrySuccess<string_t>(configStr);
}

} // detail

}} // score::memcached

