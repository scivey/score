#include "score_redis/hiredis_adapter/hiredis_adapter.h"
#include "score_redis/LLRedisClient.h"
#include <event2/event.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>

namespace score { namespace redis { namespace hiredis_adapter {

void scoreLibeventReadEvent(int fd, short event, void *arg) {
    ((void)fd); ((void)event);
    scoreLibeventEvents *e = (scoreLibeventEvents*)arg;
    redisAsyncHandleRead(e->context);
}

void scoreLibeventWriteEvent(int fd, short event, void *arg) {
    ((void)fd); ((void)event);
    scoreLibeventEvents *e = (scoreLibeventEvents*)arg;
    redisAsyncHandleWrite(e->context);
}

void scoreLibeventAddRead(void *privdata) {
    scoreLibeventEvents *e = (scoreLibeventEvents*)privdata;
    event_add(&e->rev,NULL);
}

void scoreLibeventDelRead(void *privdata) {
    scoreLibeventEvents *e = (scoreLibeventEvents*)privdata;
    event_del(&e->rev);
}

void scoreLibeventAddWrite(void *privdata) {
    scoreLibeventEvents *e = (scoreLibeventEvents*)privdata;
    event_add(&e->wev,NULL);
}

void scoreLibeventDelWrite(void *privdata) {
    scoreLibeventEvents *e = (scoreLibeventEvents*)privdata;
    event_del(&e->wev);
}

void scoreLibeventCleanup(void *privdata) {
    scoreLibeventEvents *e = (scoreLibeventEvents*)privdata;
    event_del(&e->rev);
    event_del(&e->wev);
    free(e);
}

int scoreLibeventAttach(LLRedisClient *client, redisAsyncContext *ac,
      struct event_base *base) {
    redisContext *c = &(ac->c);
    scoreLibeventEvents *e;

    /* Nothing should be attached when something is already attached */
    if (ac->ev.data != NULL) {
        return REDIS_ERR;
    }
    /* Create container for context and r/w events */
    e = (scoreLibeventEvents*)malloc(sizeof(*e));
    e->context = ac;
    e->client = client;

    /* Register functions to start/stop listening for events */
    ac->ev.addRead = scoreLibeventAddRead;
    ac->ev.delRead = scoreLibeventDelRead;
    ac->ev.addWrite = scoreLibeventAddWrite;
    ac->ev.delWrite = scoreLibeventDelWrite;
    ac->ev.cleanup = scoreLibeventCleanup;
    ac->ev.data = e;

    /* Initialize and install read/write events */
    event_set(&e->rev,c->fd,EV_READ, scoreLibeventReadEvent,e);
    event_set(&e->wev,c->fd,EV_WRITE, scoreLibeventWriteEvent,e);
    event_base_set(base,&e->rev);
    event_base_set(base,&e->wev);
    return REDIS_OK;
}

}}} // score::redis::hiredis_adapter

