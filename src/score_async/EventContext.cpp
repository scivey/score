#include "score_async/EventContext.h"
#include "score_async/EvBase.h"
#include "score_async/wheel/HTimerWheel.h"
#include "score_async/SignalHandlerRegistry.h"
#include "score/func/Function.h"
#include "score/ScopeGuard.h"
#include "score/util/misc.h"

namespace score { namespace async {

using queues::EventDataChannel;

using control_channel_t = typename EventContext::control_channel_t;
using base_t = typename EventContext::base_t;
using wheel_t = typename EventContext::wheel_t;
using sig_registry_t = typename EventContext::sig_registry_t;

EventContext::ControlMessage::ControlMessage(){}
EventContext::ControlMessage::ControlMessage(work_cb_t&& workFunc)
  : work(std::forward<work_cb_t>(workFunc)){}

EventContext::EventContext(){}

base_t* EventContext::getBase() {
  return base_.get();
}

wheel_t* EventContext::getWheel() {
  return wheel_.get();
}

sig_registry_t* EventContext::getSignalRegistry() {
  return sigRegistry_.get();
}

void EventContext::bindDataChannel(std::shared_ptr<EventDataChannel> channel) {
  EventDataChannelHandle chanHandle;
  chanHandle.channel = channel;
  chanHandle.readEvent.reset(CallbackEvent::createNewEvent(
    base_.get(), channel->getQueue()->getFDNum().value(), EV_READ
  ));
  chanHandle.readEvent->setReadHandler([this, channel]() {
    EventDataChannel::Message message;
    for (;;) {
      auto readResult = channel->getQueue()->tryRead(message);
      readResult.throwIfFailed();
      if (!readResult.value()) {
        break;
      }
      DCHECK(!!message.work);
      message.work();
    }
  });
  chanHandle.readEvent->add();
  dataChannels_.insert(std::make_pair(
    channel->getSenderID(), std::move(chanHandle)
  ));
}

void EventContext::bindControlChannel() {
  CHECK(!controlEvent_);
  CHECK(!!controlChannel_ && !!base_);
  controlEvent_.reset(CallbackEvent::createNewEvent(
    base_.get(), controlChannel_->getFDNum().value(), EV_READ
  ));
  controlEvent_->setReadHandler([this]() {
    ControlMessage message;
    for (;;) {
      auto readResult = controlChannel_->tryRead(message);
      readResult.throwIfFailed();
      if (!readResult.value()) {
        break;
      }
      DCHECK(!!message.work);
      message.work();
    }
  });
  controlEvent_->add();
}

EventContext* EventContext::createNew() {
  auto ctx = new EventContext;
  auto guard = score::makeGuard([ctx]() {
    delete ctx;
  });
  ctx->base_ = util::createUnique<base_t>();
  wheel_t::WheelSettings wheelSettings {
    {10, 10},
    {100, 10},
    {1000, 60},
    {60000, 60}
  };
  ctx->wheel_ = util::createUnique<wheel_t>(ctx->base_.get(), wheelSettings);
  ctx->wheel_->start();
  ctx->sigRegistry_ = util::createUnique<sig_registry_t>(ctx->base_.get());
  ctx->controlChannel_ = util::createUnique<control_channel_t>();
  ctx->bindControlChannel();
  guard.dismiss();
  return ctx;
}

void EventContext::runSoon(work_cb_t&& work) {
  work_cb_t toRun { std::forward<work_cb_t>(work)};
  wheel_->addOneShot(std::move(toRun), std::chrono::milliseconds{5});
}

Try<Unit> EventContext::threadsafeTrySendControlMessage(ControlMessage&& message) {
  return controlChannel_->tryLockAndSend(std::forward<ControlMessage>(message));
}

Try<Unit> EventContext::threadsafeRegisterDataChannel(
    std::shared_ptr<EventDataChannel> dataChannel) {
  return controlChannel_->tryLockAndSend(ControlMessage{[dataChannel, this]() {
    this->bindDataChannel(dataChannel);
  }});
}



}} // score::async
