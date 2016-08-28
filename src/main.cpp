#include <glog/logging.h>
// #include <spdlog/spdlog.h>
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/ReactorThread.h"
#include "aliens/reactor/SignalFd.h"
#include "aliens/reactor/TimerFd.h"
#include "aliens/reactor/EventFd.h"
#include "aliens/reactor/TCPSocket.h"
#include "aliens/reactor/AcceptSocketTask.h"
#include "aliens/reactor/ClientSocketTask.h"
#include "aliens/reactor/ServerSocketTask.h"
#include "aliens/reactor/TimerSettings.h"
#include "aliens/reactor/FdHandlerBase.h"

#include "aliens/async/ErrBack.h"
#include "aliens/async/VoidCallback.h"
#include "aliens/async/Callback.h"
#include "aliens/exceptions/macros.h"
#include <signal.h>

using namespace std;
using namespace aliens::async;
using namespace aliens::reactor;


// template<typename T>
// class FdHandlerBase {
//  public:
//   friend class ReflectedEpollTask<T>;
//   using EpollTask = ReflectedEpollTask<T>;
//  protected:
//   FileDescriptor fd_;
//   EpollTask epollTask_;
//   T* getThis() {
//     return (T*) this;
//   }
//  public:
//   void onReadable() {
//     getThis()->triggerReadable();
//   }
//   void onWritable() {
//     getThis()->triggerWritable();
//   }
//   void onError() {
//     getThis()->triggerError();
//   }
//   EpollTask* getEpollTask() {
//     return &epollTask_;
//   }
//   FdHandlerBase(FileDescriptor &&fd)
//     : fd_(std::forward<FileDescriptor>(fd)) {}
// };

// class TcpAcceptHandler: public ReactorHandlerBase<TcpAcceptHandler> {
//  public:
//   class EventHandler {
//    public:
//     virtual void onReadable() = 0;
//     virtual void onWritable() = 0;
//     virtual void onError() = 0;
//   };
//   BetterTcpHandler(FileDescriptor &&fd)
//     : ReactorHandlerBase(std::forward<FileDescriptor>(fd)) {}
//   void triggerReadable() {

//   }
//   void triggerWritable() {

//   }
//   void triggerError() {

//   }
// };



int main() {
  google::InstallFailureSignalHandler();
  auto react = ReactorThread::createShared();
  react->start();
  react->runInEventThread([react]() {
    react->stop([](const ErrBack::except_option& err) {
      CHECK(!err.hasValue());
      LOG(INFO) << "stopped";
    });
  });
  react->join();
  LOG(INFO) << "here...";
}
