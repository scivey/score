## todo

### score_core
#### general
* need a StringPiece implementation
* integrate `fmtlib` to replace calls to `folly::format`.
#### encoding detection + conversion
* split this into a separate `score_encoding` library.
* convert current `EncodingNormalizer` to a stream-based interface; move existing one-shot interface (handling single input buffer -> single output buffer) to a wrapper class.
* consider replacing vendored Libcharsetdetect with Google's [compact_enc_det](https://github.com/google/compact_enc_det).  Seems like cleaner code and has better licensing - but haven't checked if it covers as many encodings.
#### hashing
* split into separate score_hash library.


### score_async
* need lift/drop on `STFuture.then()` handlers (unwrapping of future-typed return values)
* MTPromise/MTFuture implementations
* cpu thread pool implementation
#### HTimerWheel
* if sticking with current implementation, make it more efficient.  It's currently hierarchical, but not hashed.  Resolution is also unnecessarily high: I've seen others use just 32 bits per timeout value.
* consider adapting third party implementation: either Folly's HHTimerWheel, or the `timeout` library [here](https://github.com/wahern/timeout).



### score_curl
* convert EventCurler to use an EventHandler object with onRead/etc handlers; move existing on-done-callback interface to a wrapper class


### score_html
* convert the current (and convoluted) callback-based search to use BFS+DFS iterator classes


### score_extract
#### goose
* switch most of the string matching parts here to use RE2

### score_nlp
* Complete integration of score::nlp::Language with CLD2.  CLD2 wrapper should only return the score Language type.
* Need easier conversion between all of: language code (char*), English human-friendly language name, `score::nlp::Language` type.

### score_memcached
* Add async client
* Add mget/mset to sync client.

### score_redis
* LLRedisClient: still missing a few commands (listed in the LLRedisClient header).
* Not currently handling get/set of binary data types (hiredis has built-in support for this with the `%b` format arg).
#### high-level callback redis client
* create high-level (but still callback-based) wrapper around LLRedisClient.
* should narrow returned types from `RedisDynamicResponse` based on the type of call: e.g. a call to `llen` can call back with a `Try<int>` instead of the general `RedisDynamicResponse`.
* add corresponding high-level RedisSubscription type, which should convert the RedisDynamicResponse into a more meaningful `Message` type.
* multi/pipeline interface



### additional libraries
* `score_dns`: I have a working libevent adapter for libcares in the `evscratch` repo.  Another option is [this library](https://github.com/wahern/dns), which seems lighter-weight.  Unclear if this covers all of DNS protocol.  Also unclear how simple it is to integrate with libevent.  The main motivation for using that over cares: cares seems to do a lot of things behind the scenes, like managing its own thread pool.  I consider this rude.
* also have proof-of-concept implementations of zmq and libssh2 integration in `evscratch` repo.
