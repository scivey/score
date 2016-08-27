#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>
#include <fcntl.h>
#include <netdb.h>
#include <cstdio>
#include <cstdlib>

#include <stdexcept>
#include <vector>
#include <queue>
#include <atomic>

#include <glog/logging.h>
#include <gtest/gtest.h>

#include "aliens/ScopeGuard.h"
#include "aliens/locks/Synchronized.h"
#include "aliens/async/ErrBack.h"
#include "aliens/async/VoidCallback.h"
#include "aliens/Maybe.h"
#include "aliens/MoveWrapper.h"
#include "aliens/FixedBuffer.h"


using namespace std;
using aliens::async::ErrBack;
using aliens::async::VoidCallback;
using aliens::locks::Synchronized;
using aliens::MoveWrapper;
using aliens::Buffer;

class BaseError: public std::runtime_error {
 public:
  template<typename T>
  BaseError(const T& msg): std::runtime_error(msg){}
};

class SystemError: public BaseError {
 protected:
  int errno_ {0};
 public:
  // SystemError(int err): BaseError(strerror(err)), errno_(err) {}
  SystemError(int err, const std::string &msg)
    : BaseError(msg), errno_(err) {}
  SystemError(int err)
    : BaseError(strerror(err)), errno_(err){}
  int getErrno() const {
    return errno_;
  }
  static SystemError fromErrno(int err, const std::string &msg) {
    std::ostringstream oss;
    oss << "ERR [" << err << ":'" << strerror(err) << "'] : '" << msg << "'";
    return SystemError(err, oss.str());
  }
};

#define CHECK_SYSCALL(expr) \
  do { \
    if ((expr) < 0) { \
      throw SystemError(errno); \
    } \
  } while (0); \

#define CHECK_SYSCALL2(expr, msg) \
  do { \
    if ((expr) < 0) { \
      throw SystemError::fromErrno(errno, msg); \
    } \
  } while (0); \


class FileDescriptor {
 protected:
  int fd_ {0};
  FileDescriptor(){}
  FileDescriptor(int fd): fd_(fd){}
  FileDescriptor(const FileDescriptor&) = delete;
  FileDescriptor& operator=(const FileDescriptor&) = delete;

  void maybeClose() {
    if (fd_ > 0) {
      ::close(fd_);
      fd_ = 0;
    }
  }
 public:
  static FileDescriptor fromIntExcept(int fd) {
    CHECK(fd >= 0);
    return FileDescriptor(fd);
  }
  FileDescriptor(FileDescriptor &&other): fd_(other.fd_) {
    other.fd_ = 0;
  }
  FileDescriptor& operator=(FileDescriptor &&other) {
    int temp = other.fd_;
    other.fd_ = fd_;
    fd_ = temp;
    return *this;
  }
  ~FileDescriptor() {
    maybeClose();
  }
  int get() {
    return fd_;
  }
  bool valid() const {
    return fd_ > 0;
  }
  explicit operator bool() const {
    return valid();
  }
  void makeNonBlocking() {
    CHECK(valid());
    int flags, status;
    flags = fcntl(fd_, F_GETFL, 0);
    if (flags == -1) {
      throw SystemError(errno);
    }
    flags |= O_NONBLOCK;
    status = fcntl(fd_, F_SETFL, flags);
    CHECK_SYSCALL(status);
  }
  void close() {
    CHECK(valid());
    CHECK_SYSCALL(::close(fd_));
  }
};

class EpollFd {
 protected:
  FileDescriptor fd_;
  EpollFd(FileDescriptor &&descriptor)
    : fd_(std::forward<FileDescriptor>(descriptor)) {}

 public:
  static EpollFd create() {
    int fd = epoll_create1(EPOLL_CLOEXEC);
    CHECK_SYSCALL(fd);
    return EpollFd(FileDescriptor::fromIntExcept(fd));
  }
  int get() {
    CHECK(!!fd_);
    return fd_.get();
  }
};

bool epollEventHasError(const epoll_event *evt) {
  return evt->events & EPOLLERR
    || evt->events & EPOLLHUP
    || (!evt->events & EPOLLIN);
}

bool epollEventHasError(const epoll_event &evt) {
  return epollEventHasError(&evt);
}


class EpollReactor {
 public:
  static const size_t kMaxEvents = 1024;
  class Task {
   protected:
    EpollReactor *reactor_ {nullptr};
    void setReactor(EpollReactor *reactor) {
      reactor_ = reactor;
    }
   public:
    friend class EpollReactor;
    EpollReactor* getReactor() const {
      return reactor_;
    }
    virtual int getFd() = 0;
    virtual void onError() = 0;
    virtual void onEvent() = 0;
  };
 protected:
  bool running_ {false};
  std::vector<Task*> tasks_;
  epoll_event events_[kMaxEvents];
  EpollFd epollFd_;
  EpollReactor(EpollFd &&fd)
    : epollFd_(std::forward<EpollFd>(fd)) {
    memset((void*) &events_, '\0', sizeof(events_));
  }
  static EpollReactor* createPtr() {
    return new EpollReactor(EpollFd::create());
  }
 public:
  static EpollReactor create() {
    return EpollReactor(EpollFd::create());
  }
  static std::unique_ptr<EpollReactor> createUnique() {
    return std::unique_ptr<EpollReactor>(createPtr());
  }
  void addTask(Task *task) {
    epoll_event evt;
    evt.data.ptr = (void*) task;
    evt.events = EPOLLIN | EPOLLET;
    CHECK_SYSCALL(epoll_ctl(
      epollFd_.get(), EPOLL_CTL_ADD, task->getFd(), &evt
    ));
    task->setReactor(this);
    tasks_.push_back(task);
  }
  int runOnce() {
    int nEvents = epoll_wait(epollFd_.get(), events_, kMaxEvents, 20);
    for (size_t i = 0; i < nEvents; i++) {
      auto task = (Task*) events_[i].data.ptr;
      if (epollEventHasError(events_[i])) {
        task->onError();
        continue;
      } else {
        task->onEvent();
      }
    }
    return nEvents;
  }
  void loopForever() {
    running_ = true;
    for (;;) {
      runOnce();
      if (!running_) {
        break;
      }
    }
  }
  using msec = std::chrono::milliseconds;
  void runForDuration(msec duration) {
    auto start = std::chrono::system_clock::now().time_since_epoch();
    for (;;) {
      runOnce();
      auto now = std::chrono::system_clock::now().time_since_epoch();
      auto elapsed = now - start;
      if (elapsed >= duration) {
        break;
      }
    }
  }
  void stop() {
    CHECK(running_);
    running_ = false;
  }
  bool isRunning() const {
    return running_;
  }
};


class SocketAddr {
 protected:
  std::string host_;
  short port_;
 public:
  SocketAddr(const std::string &host, short port)
    : host_(host), port_(port){}
  const std::string& getHost() const {
    return host_;
  }
  short getPort() const {
    return port_;
  }
  struct sockaddr_in to_sockaddr_in() const {
    struct sockaddr_in result;
    memset(&result, 0, sizeof(result));
    result.sin_family = AF_INET;
    result.sin_port = htons(port_);
    if (inet_aton(host_.c_str(), &result.sin_addr) == 0) {
      throw BaseError("invalid address.");
    }
    return result;
  }
};

class AcceptSocketTask;
class ServerSocketTask;
class ClientSocketTask;

class TCPSocket {
 protected:
  FileDescriptor fd_;
  short localPort_ {0};
  short remotePort_ {0};
  std::string remoteHost_ {""};
  TCPSocket(FileDescriptor &&fd)
    : fd_(std::forward<FileDescriptor>(fd)){}

  int getFdNo() {
    return fd_.get();
  }
 public:
  friend class AcceptSocketTask;
  friend class ServerSocketTask;
  friend class ClientSocketTask;
  static TCPSocket fromAccepted(FileDescriptor &&fd, const char* remoteHost, const char *remotePort) {
    TCPSocket sock(std::forward<FileDescriptor>(fd));
    sock.remoteHost_ = remoteHost;
    int remPort = atol(remotePort);
    CHECK(remPort <= 65535);
    sock.remotePort_ = (short) remPort;
    return sock;
  }
  static TCPSocket connect(SocketAddr addr) {
    const int protocolPlaceholder = 0;
    int sockFd = ::socket(
      AF_INET,
      SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
      protocolPlaceholder
    );
    LOG(INFO) << "made socket.";

    CHECK_SYSCALL2(sockFd, "socket()");
    LOG(INFO) << "connecting...";
    auto addrIn = addr.to_sockaddr_in();
    socklen_t addrLen = sizeof addrIn;
    int rc = ::connect(sockFd, (struct sockaddr*) &addrIn, addrLen);
    if (rc < 0 && errno != EINPROGRESS) {
      throw SystemError::fromErrno(errno, "connect()");
    }
    LOG(INFO) << "connected.";
    auto desc = FileDescriptor::fromIntExcept(sockFd);
    TCPSocket sock(std::move(desc));
    sock.remotePort_ = addr.getPort();
    sock.remoteHost_ = addr.getHost();
    return sock;
  }
  static TCPSocket bindPort(short portno) {
    ostringstream oss;
    oss << portno;
    auto portStr = oss.str();
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; // either ipv4 or ipv6
    hints.ai_socktype = SOCK_STREAM; // tcp
    hints.ai_flags = AI_PASSIVE; // any interface
    struct addrinfo *result, *rp;
    aliens::ScopeGuard guard([&result]() {
      if (result) {
        freeaddrinfo(result);
      }
    });
    CHECK_SYSCALL(getaddrinfo(nullptr, portStr.c_str(), &hints, &result));
    int status, sfd;
    for (rp = result; rp != nullptr; rp = rp->ai_next) {
      sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (sfd == -1) {
        continue;
      }
      {
        int shouldReuse = 1;
        CHECK_SYSCALL(setsockopt(
          sfd, SOL_SOCKET, SO_REUSEADDR,
          (const char*) &shouldReuse,
          sizeof(shouldReuse)
        ));
        CHECK_SYSCALL(setsockopt(
          sfd, SOL_SOCKET, SO_REUSEPORT,
          (const char*) &shouldReuse,
          sizeof(shouldReuse)
        ));
      }
      status = bind(sfd, rp->ai_addr, rp->ai_addrlen);
      if (status == 0) {
        break;
      }
      close(sfd);
    }
    if (!rp) {
      throw BaseError("could not bind.");
    }
    auto desc = FileDescriptor::fromIntExcept(sfd);
    desc.makeNonBlocking();
    TCPSocket sock(std::move(desc));
    sock.localPort_ = portno;
    return sock;
  }
  bool valid() const {
    return fd_.valid();
  }
  void listen() {
    CHECK_SYSCALL(::listen(fd_.get(), SOMAXCONN));
  }
  void stop() {
    fd_.close();
  }
};


class ServerSocketTask: public EpollReactor::Task {
 protected:
  TCPSocket sock_;
  const size_t kReadBuffSize = 512;
  void doRead() {
    for (;;) {
      char buff[kReadBuffSize];
      ssize_t count = read(sock_.getFdNo(), buff, kReadBuffSize);
      if (count == -1) {
        if (errno == EAGAIN) {
          break;
        }
      } else if (count == 0) {
        onEOF();
        break;
      } else {
        onRead(buff, count);
      }
    }
  }
 public:
  ServerSocketTask(TCPSocket &&sock)
    : sock_(std::forward<TCPSocket>(sock)) {}
  virtual void onEvent() override {
    LOG(INFO) << "ServerSocketTask onEvent";
    doRead();
  }
  virtual void onError() override {
    LOG(INFO) << "ServerSocketTask onError";
  }
  virtual void onRead(char* buff, ssize_t buffLen) {
    LOG(INFO) << "onRead! [" << buffLen << "] : '" << buff << "'";
  }
  virtual void onEOF() {
    LOG(INFO) << "onEOF!";
  }
  int getFd() override {
    return sock_.fd_.get();
  }
};

class AcceptSocketTask: public EpollReactor::Task {
 protected:
  TCPSocket sock_;
  void doAccept() {
    for (;;) {
      struct sockaddr inAddr;
      socklen_t inLen;
      char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
      inLen = sizeof inAddr;
      int inFd = accept(getFd(), &inAddr, &inLen);
      if (inFd == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          // all incoming connections processed.
          break;
        } else {
          onAcceptError(errno);
          break;
        }
      }
      int status = getnameinfo(
        &inAddr, inLen,
        hbuf, sizeof hbuf,
        sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV);
      if (status == 0) {
        onAcceptSuccess(inFd, hbuf, sbuf);
      } else {
        onAcceptError(errno);
      }
    }
  }
 public:
  AcceptSocketTask(TCPSocket &&sock)
    : sock_(std::forward<TCPSocket>(sock)) {}

  void onEvent() override {
    LOG(INFO) << "onEvent! Accepting....";
    doAccept();
  }
  void onError() override {
    LOG(INFO) << "onError!";
  }
  int getFd() override {
    return sock_.fd_.get();
  }
  TCPSocket& getSocket() {
    return sock_;
  }
  virtual void onAcceptSuccess(int inFd, const char *host, const char *port) {
    LOG(INFO) << "onAcceptSuccess : [" << inFd << "] " << host << ":" << port;
    auto fd = FileDescriptor::fromIntExcept(inFd);
    fd.makeNonBlocking();
    auto sock = TCPSocket::fromAccepted(
      std::move(fd), host, port
    );
    auto newTask = new ServerSocketTask(std::move(sock));
    LOG(INFO) << "made new task.";
    getReactor()->addTask(newTask);
  }
  virtual void onAcceptError(int err) {
    LOG(INFO) << "onAcceptError : " << strerror(err);
  }
};






class ClientSocketTask: public EpollReactor::Task {
 public:
  class EventHandler {
   protected:
    ClientSocketTask *task_ {nullptr};
    void setTask(ClientSocketTask *task) {
      task_ = task;
    }
   public:
    friend class ClientSocketTask;
    virtual ClientSocketTask* getTask() {
      return task_;
    }
    virtual void onConnectSuccess() = 0;
    virtual void onConnectError(const std::exception&) = 0;
    virtual void onWritable() = 0;
    virtual void onReadable() = 0;
    virtual void write(std::unique_ptr<Buffer> buff, ErrBack &&cb) {
      task_->write(std::move(buff), std::forward<ErrBack>(cb));
    }
    virtual void readInto(std::unique_ptr<Buffer> buff, ErrBack &&cb) {
      task_->readInto(std::move(buff), std::forward<ErrBack>(cb));
    }
  };
  friend class EventHandler;
 protected:
  TCPSocket sock_;
  EventHandler *handler_ {nullptr};
  void readInto(std::unique_ptr<Buffer> buff, ErrBack &&cb) {
    LOG(INFO) << "readInto()";
  }
  void write(std::unique_ptr<Buffer> buff, ErrBack &&cb) {
    LOG(INFO) << "write()";
  }
 public:
  ClientSocketTask(TCPSocket &&sock, EventHandler *handler)
    : sock_(std::forward<TCPSocket>(sock)), handler_(handler){
    handler_->setTask(this);
  }
  virtual void onEvent() override {
    LOG(INFO) << "ClientSocketTask onEvent";
    // doRead();
  }
  virtual void onError() override {
    LOG(INFO) << "ClientSocketTask onError";
  }
  int getFd() override {
    return sock_.getFdNo();
  }
};

class ReactorThread : public std::enable_shared_from_this<ReactorThread> {
 protected:
  std::unique_ptr<EpollReactor> reactor_ {nullptr};
  std::unique_ptr<std::thread> thread_ {nullptr};
  ReactorThread(){}
  std::atomic<bool> running_ {false};
  aliens::Maybe<ErrBack> onStopped_;
  Synchronized<std::queue<VoidCallback>> toRun_;
 public:
  static std::shared_ptr<ReactorThread> createShared() {
    std::shared_ptr<ReactorThread> instance{new ReactorThread};
    instance->reactor_ = EpollReactor::createUnique();
    return instance;
  }
  EpollReactor* getReactor() {
    return reactor_.get();
  }
  bool isRunning() const {
    return running_.load();
  }
  void start() {
    CHECK(!isRunning());
    bool expected = false;
    bool desired = true;
    if (running_.compare_exchange_strong(expected, desired)) {
      auto self = shared_from_this();
      thread_.reset(new std::thread([this, self]() {
        while (isRunning()) {
          reactor_->runForDuration(std::chrono::milliseconds(50));
          auto queueHandle = toRun_.getHandle();
          while (!queueHandle->empty()) {
            VoidCallback func = std::move(queueHandle->front());
            queueHandle->pop();
            func();
          }
        }
        if (onStopped_.hasValue()) {
          onStopped_.value()();
        }
      }));
    } else {
      throw BaseError("already started!");
    }
  }
  void stop(ErrBack &&cb) {
    if (!isRunning()) {
      cb(BaseError("not running!"));
      return;
    }
    onStopped_.assign(std::move(cb));
    bool expected = true;
    bool desired = false;
    if (running_.compare_exchange_strong(expected, desired)) {
      ; // do nothing; cb will be called by the loop in `start()`
    } else {
      onStopped_.value()(BaseError("someone else stopped before we could. race condition?"));
    }
  }

 protected:
  // NB must be called from event loop thread.
  void addTaskImpl(EpollReactor::Task *task) {
    reactor_->addTask(task);
  }

 public:
  void addTask(EpollReactor::Task *task) {
    auto self = shared_from_this();
    runInEventThread([self, this, task]() {
      addTaskImpl(task);
    });
  }
  void addTask(EpollReactor::Task *task, VoidCallback &&cb) {
    auto self = shared_from_this();
    auto cbWrapper = aliens::makeMoveWrapper(std::forward<VoidCallback>(cb));
    runInEventThread([self, this, task, cbWrapper]() {
      addTaskImpl(task);
      aliens::MoveWrapper<VoidCallback> unwrapped = cbWrapper;
      VoidCallback movedCb = unwrapped.move();
      movedCb();
    });
  }
  void runInEventThread(VoidCallback &&cb) {
    auto handle = toRun_.getHandle();
    handle->push(std::move(cb));
  }
  void runInEventThread(VoidCallback &&cb, ErrBack &&onFinish) {
    auto cbWrapper = aliens::makeMoveWrapper(
      std::forward<VoidCallback>(cb)
    );
    auto doneWrapper = aliens::makeMoveWrapper(
      std::forward<ErrBack>(onFinish)
    );
    auto self = shared_from_this();
    runInEventThread([this, self, cbWrapper, doneWrapper]() {
      MoveWrapper<VoidCallback> movedCb = cbWrapper;
      VoidCallback unwrappedCb = movedCb.move();
      MoveWrapper<ErrBack> movedDoneCb = doneWrapper;
      ErrBack unwrappedDoneCb = movedDoneCb.move();
      bool raised = false;
      try {
        unwrappedCb();
      } catch (const std::exception &ex) {
        raised = true;
        unwrappedDoneCb(ex);
      }
      if (!raised) {
        unwrappedDoneCb();
      }
    });
  }
  void join() {
    if (!isRunning()) {
      return;
    }
    while (running_.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    thread_->join();
  }
};

TEST(TestEpollReactor, TestConstruction) {
  auto reactor = EpollReactor::create();
  EXPECT_FALSE(reactor.isRunning());
}

TEST(TestEpollReactor, TestRun1) {
  auto reactor = EpollReactor::create();
  reactor.runOnce();
  EXPECT_TRUE(true);
  LOG(INFO) << "here";
}

TEST(TestTCPSocket, TestSanity) {
  auto sock = TCPSocket::bindPort(9019);
  CHECK(sock.valid());
}



TEST(TestEpollReactor, TestRun2) {
  auto reactor = EpollReactor::create();
  auto sock = TCPSocket::bindPort(9022);
  auto task = std::unique_ptr<AcceptSocketTask>(
    new AcceptSocketTask(std::move(sock))
  );
  task->getSocket().listen();
  reactor.addTask(task.get());
  reactor.runForDuration(std::chrono::milliseconds{20});
  EXPECT_TRUE(true);
  task->getSocket().stop();
  LOG(INFO) << "here";
}


TEST(TestEpollReactor, TestRun3) {
  auto reactorThread = ReactorThread::createShared();
  reactorThread->start();
  auto sock = TCPSocket::bindPort(9025);
  auto task = std::unique_ptr<AcceptSocketTask>(
    new AcceptSocketTask(std::move(sock))
  );
  task->getSocket().listen();
  reactorThread->addTask(task.get(), [](){
    LOG(INFO) << "added task!";
  });
  LOG(INFO) << "sleeping...";
  this_thread::sleep_for(chrono::milliseconds(100));
  std::atomic<bool> done {false};
  reactorThread->runInEventThread([&task, &reactorThread, &done]() {
    task->getSocket().stop();
    reactorThread->stop([&done](const aliens::Maybe<std::exception> &err) {
      done.store(true);
    });
  });
  LOG(INFO) << "waiting for stop..";
  reactorThread->join();
  LOG(INFO) << "end.";
}


void joinAtomic(std::atomic<bool> &done) {
  while (!done.load()) {
    this_thread::sleep_for(chrono::milliseconds(50));
  }
}

class ClientHandler : public ClientSocketTask::EventHandler {
 public:
  void onConnectSuccess() override {
    LOG(INFO) << "onConnectSuccess!";
  }
  void onConnectError(const std::exception &ex) override {
    LOG(INFO) << "onConnectError : " << ex.what();
  }
  void onWritable() override {
    LOG(INFO) << "onWritable.";
  }
  void onReadable() override {
    LOG(INFO) << "onWritable.";
  }
};

TEST(TestEpollReactor, TestRun4) {
  auto reactorThread = ReactorThread::createShared();
  reactorThread->start();
  auto sock = TCPSocket::bindPort(9025);
  auto task = std::unique_ptr<AcceptSocketTask>(
    new AcceptSocketTask(std::move(sock))
  );
  task->getSocket().listen();
  std::atomic<bool> serverStarted {false};
  reactorThread->addTask(task.get(), [&serverStarted](){
    LOG(INFO) << "added task!";
    serverStarted.store(true);
  });
  joinAtomic(serverStarted);
  LOG(INFO) << "sleeping...";
  this_thread::sleep_for(chrono::milliseconds(200));


  auto clientSock = TCPSocket::connect(SocketAddr("127.0.0.1", 9025));
  auto clientTask = new ClientSocketTask(std::move(clientSock), new ClientHandler);
  reactorThread->addTask(clientTask, []() {
    LOG(INFO) << "added client!";
  });
  LOG(INFO) << "sleeping.....";
  this_thread::sleep_for(chrono::milliseconds(1000));
  std::atomic<bool> done {false};
  LOG(INFO) << "stopping..";
  reactorThread->runInEventThread([&task, &reactorThread, &done]() {
    LOG(INFO) << "here..";
    task->getSocket().stop();
    reactorThread->stop([&done](const aliens::Maybe<std::exception> &err) {
      done.store(true);
    });
  });
  LOG(INFO) << "waiting for stop..";
  reactorThread->join();
  LOG(INFO) << "end.";
}