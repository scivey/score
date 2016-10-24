#include "score/memcached/SyncMCClient.h"
#include "score/memcached/MemcachedConfig.h"
#include "score/memcached/MemcachedError.h"
#include "score/ScopeGuard.h"
#include "score/ExceptionWrapper.h"
#include "score/Try.h"
#include "score/util/try_helpers.h"
#include <libmemcached/memcached.h>
#include <glog/logging.h>

using namespace std;

namespace score { namespace memcached {

using string_t = typename SyncMCClient::string_t;

SyncMCClient::SyncMCClient(const MemcachedConfig& config)
  : config_(config){}

SyncMCClient::SyncMCClient(MemcachedConfig&& config)
  : config_(std::forward<MemcachedConfig>(config)){}

SyncMCClient::SyncMCClient(SyncMCClient&& other)
  : config_(other.config_), mcHandle_(other.mcHandle_) {
  other.mcHandle_ = nullptr;
}

SyncMCClient& SyncMCClient::operator=(
    SyncMCClient &&other) {
  std::swap(config_, other.config_);
  std::swap(mcHandle_, other.mcHandle_);
  return *this;
}

Try<Unit> SyncMCClient::connect() {
  if (isConnected()) {
    return util::makeTryFailure<Unit, AlreadyConnected>(
      "memcached client already connected"
    );
  }
  if (!config_.hasAnyServers()) {
    return util::makeTryFailure<Unit, ConfigurationError>(
      "No servers are configured."
    );
  }
  auto confStrOpt = config_.toConfigString();
  if (confStrOpt.hasException()) {
    return Try<Unit>{std::move(confStrOpt.exception())};
  }
  ScopeGuard guard([this]() {
    if (mcHandle_) {
      free(mcHandle_);
      mcHandle_ = nullptr;
    }
  });
  mcHandle_ = ::memcached(
    confStrOpt.value().c_str(),
    confStrOpt.value().size()
  );
  if (!mcHandle_) {
    return util::makeTryFailure<Unit, ConnectionError>(
      "libmemcached's memcached() function "
      "returned a nullptr with no explanation."
    );
  }
  if (memcached_last_error(mcHandle_) != MEMCACHED_SUCCESS) {
    return util::makeTryFailure<Unit, ConnectionError>(
      memcached_last_error_message(mcHandle_)
    );
  }
  guard.dismiss();
  return util::makeTrySuccess<Unit>();
}

bool SyncMCClient::isConnected() const {
  return !!mcHandle_;
}

void SyncMCClient::connectExcept() {
  connect().throwIfFailed();
}


MemcachedConfig& SyncMCClient::getConfig() {
  return config_;
}

const MemcachedConfig& SyncMCClient::getConfig() const {
  return config_;
}

using get_result_t = SyncMCClient::get_result_t;

get_result_t SyncMCClient::get(const string_t& key) {
  DCHECK(isConnected());
  memcached_return_t errCode;
  size_t valLen {0};
  uint32_t flags {0};
  auto valBuff = memcached_get(mcHandle_, key.c_str(), key.size(),
    &valLen, &flags, &errCode);
  ScopeGuard guard([&valBuff]() {
    if (valBuff != nullptr) {
      free(valBuff);
    }
  });
  if (errCode != MEMCACHED_SUCCESS) {
    return util::makeTryFailure<Optional<string_t>, ProtocolError>(
      memcached_last_error_message(mcHandle_)
    );
  }
  Optional<string_t> result;
  if (valLen > 0) {
    result.assign(string_t {valBuff, valLen});
  }
  return util::makeTrySuccess<Optional<string_t>>(result);
}

using set_result_t = SyncMCClient::set_result_t;

set_result_t SyncMCClient::set(const string_t& key,
    const string_t& val, time_t ttl) {
  DCHECK(isConnected());
  uint32_t flags {0};
  auto rc = memcached_set(mcHandle_,
    key.c_str(), key.size(),
    val.c_str(), val.size(),
    ttl, flags
  );
  if (rc != MEMCACHED_SUCCESS) {
    return util::makeTryFailure<Unit, ProtocolError>(
      memcached_last_error_message(mcHandle_)
    );
  }
  return util::makeTrySuccess<Unit>();
}

}} // score::memcached