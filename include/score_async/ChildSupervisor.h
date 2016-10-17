#pragma once

#include <vector>
#include <sys/wait.h>
#include <folly/FBString.h>
#include <folly/FBVector.h>
#include <glog/logging.h>
#include "score_async/EventContext.h"
#include "evs/exceptions/PosixError.h"
#include "score/macros.h"
#include "score/logging.h"

namespace score { namespace async {


class ChildSupervisor {
 public:
  using context_t = EventContext;
  using context_ptr_t = std::shared_ptr<context_t>;
  using string_t = folly::fbstring;
  using string_vector_t = folly::fbvector<string_t>;

  struct ChildArgs {
    string_t filename;
    string_vector_t args;
    ChildArgs(){}
    ChildArgs(string_t&& name, string_vector_t&& argSeq): filename(name), args(argSeq){}
  };

  class ChildHandle {
   public:
    using args_t = ChildArgs;
   protected:
    pid_t pid_ {0};
    args_t args_;
    int exitCode_ {0};
    bool finished_ {false};
   public:
    ChildHandle(){}
    bool good() const {
      return pid_ > 0;
    }
    operator bool() const {
      return good();
    }
    ChildHandle(pid_t childPid, const args_t& args)
      : pid_(childPid), args_(args){}
    pid_t getPID() const {
      return pid_;
    }
    const args_t& getArgs() const {
      return args_;
    }
    void markFinished() {
      CHECK(!finished_);
      finished_ = true;
    }
    void setExitCode(int code) {
      exitCode_ = code;
    }
    int getExitCode() const {
      return exitCode_;
    }
    bool isFinished() const {
      return finished_;
    }
  };

  using child_map_t = std::unordered_map<pid_t, ChildHandle>;
  SCORE_DISABLE_COPY_AND_ASSIGN(ChildSupervisor);
 protected:
  context_ptr_t context_ {nullptr};
  child_map_t children_;
  bool started_ {false};
  ChildSupervisor(context_ptr_t ptr): context_(ptr){}
  void onSigChild() {
    int rc {0};
    auto childPid = waitpid(-1, &rc, WNOHANG);
    SCORE_CHECK_POSIX_CALL(childPid, "waitpid()");
    if (childPid == 0) {
      SCORE_INFO("onSigChild : no change found in children?.");
    } else {
      SCORE_INFO("onSigChild: {} / {}", childPid, rc);
    }
  }
 public:
  void start() {
    CHECK(!!context_);
    CHECK(!started_);
    started_ = true;
    context_->getSignalRegistry()->addHandler(SIGCHLD, [this]() {
      this->onSigChild();
    });
  }
  bool hasStarted() const {
    return started_;
  }
  static ChildSupervisor* createNew(context_ptr_t context) {
    return new ChildSupervisor(context);
  }

 protected:
  void childExec(const ChildArgs& args) {
      std::vector<const char*> argPtrs;
      argPtrs.push_back(args.filename.c_str());
      for (const auto& arg: args.args) {
        argPtrs.push_back(arg.c_str());
      }
      argPtrs.push_back(nullptr);
      auto argPtrArray = (char * const*) argPtrs.data();
      SCORE_MAKE_POSIX_CALL(execve(args.filename.c_str(), argPtrArray, nullptr));
  }
 public:
  void addChild(const ChildArgs& args) {
    CHECK(hasStarted());
    auto childPid = fork();
    SCORE_CHECK_POSIX_CALL(childPid, "fork()");
    if (childPid == 0) {
      // this is the child
      childExec(args);
    } else {
      SCORE_INFO("adding child pid: {}", childPid);
      ChildHandle childHandle { childPid, args };
      children_.insert(std::make_pair(childPid, std::move(childHandle)));
    }
  }
};


}} // score::async
