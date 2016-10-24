#include "score_redis/hiredis_adapter/hiredis_adapter.h"
#include "score_redis/hiredis_adapter/LibeventRedisContext.h"

#include "score_redis/LLRedisClient.h"
#include <event2/event.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>

namespace score { namespace redis { namespace hiredis_adapter {

using context_t = LibeventRedisContext::InnerContext;

context_t* getCtx(void *arg) {
  return (context_t*) arg;
}

void scoreLibeventReadEvent(int fd, short event, void *arg) {
  ((void)fd); ((void)event);
  redisAsyncHandleRead(getCtx(arg)->redisContext);
}

void scoreLibeventWriteEvent(int fd, short event, void *arg) {
  ((void)fd); ((void)event);
  redisAsyncHandleWrite(getCtx(arg)->redisContext);
}

void scoreLibeventAddRead(void *privdata) {
  event_add(&getCtx(privdata)->readEvent, nullptr);
}

void scoreLibeventDelRead(void *privdata) {
  event_del(&getCtx(privdata)->readEvent);
}

void scoreLibeventAddWrite(void *privdata) {
  event_add(&getCtx(privdata)->writeEvent, nullptr);
}

void scoreLibeventDelWrite(void *privdata) {
  event_del(&getCtx(privdata)->writeEvent);
}

void scoreLibeventCleanup(void *privdata) {
  auto ctx = getCtx(privdata);
  event_del(&ctx->readEvent);
  event_del(&ctx->writeEvent);
}

std::shared_ptr<LibeventRedisContext> scoreLibeventAttach(
    LLRedisClient *client, redisAsyncContext *ac, struct event_base *base) {

  redisContext *c = &(ac->c);

  /* Nothing should be attached when something is already attached */
  DCHECK(ac->ev.data == nullptr);

  auto scoreEventParent = std::make_shared<LibeventRedisContext>();
  auto scoreCtx = scoreEventParent->getInnerContext();
  scoreCtx->redisContext = ac;
  scoreCtx->clientPtr = client;

  /* Register functions to start/stop listening for events */
  ac->ev.addRead = scoreLibeventAddRead;
  ac->ev.delRead = scoreLibeventDelRead;
  ac->ev.addWrite = scoreLibeventAddWrite;
  ac->ev.delWrite = scoreLibeventDelWrite;
  ac->ev.cleanup = scoreLibeventCleanup;
  ac->ev.data = scoreCtx;

  /* Initialize and install read/write events */
  event_set(&scoreCtx->readEvent,
    c->fd,
    EV_READ,
    scoreLibeventReadEvent,
    scoreCtx
  );
  event_set(&scoreCtx->writeEvent,
    c->fd,
    EV_WRITE,
    scoreLibeventWriteEvent,
    scoreCtx
  );
  event_base_set(base, &scoreCtx->readEvent);
  event_base_set(base, &scoreCtx->writeEvent);
  return scoreEventParent;
}

}}} // score::redis::hiredis_adapter

