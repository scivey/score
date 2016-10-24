#pragma once
#include "score/Try.h"
#include "score/Unit.h"
#include "score/Optional.h"
#include "score/io/Scstring.h"
#include "score_memcached/MemcachedConfig.h"

struct memcached_st;

namespace score { namespace memcached {

class SyncMCClient {
 public:
  using string_t = score::io::Scstring;
 protected:
  MemcachedConfig config_;
  memcached_st* mcHandle_ {nullptr};
  SyncMCClient(const SyncMCClient&) = delete;
  SyncMCClient& operator=(const SyncMCClient&) = delete;

 public:
  SyncMCClient();
  SyncMCClient(const MemcachedConfig& config);
  SyncMCClient(MemcachedConfig&& config);
  SyncMCClient(SyncMCClient&&);
  SyncMCClient& operator=(SyncMCClient&&);

  MemcachedConfig& getConfig();
  const MemcachedConfig& getConfig() const;

  score::Try<score::Unit> connect();
  void connectExcept();
  bool isConnected() const;

  using get_result_t = score::Try<score::Optional<string_t>>;
  get_result_t get(const string_t& key);

  using set_result_t = score::Try<score::Unit>;
  set_result_t set(const string_t& key, const string_t& val, time_t ttl = 0);
};

}} // score::memcached
