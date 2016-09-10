#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <sys/select.h>
#include <fcntl.h>

#include <curl/curl.h>
#include <glog/logging.h>
#include <folly/io/async/EventBase.h>
#include <spdlog/spdlog.h>

#include "score/macros.h"
#include "score/curl/AsyncCurler.h"
#include "score/curl/RealCurlAPI.h"
#include "score/curl/AsyncRequestContext.h"
#include "score/folly_util/EBThread.h"

using namespace std;
using namespace score::curl;
using EBT = score::folly_util::EBThread<folly::EventBase>;


using Curler = AsyncCurler<RealCurlAPI>;

class Handler : public AsyncRequestContext<RealCurlAPI>::EventHandler {
 protected:
  char errBuff_[CURL_ERROR_SIZE];
  std::ostringstream bodyBuffer_;
  std::vector<std::string> rawHeaders_;
 public:
  char* getErrorBuffer() override {
    return errBuff_;
  }
  size_t onReadBody(char *data, size_t size, size_t nMembers) override {
    size_t dataLen = size * nMembers;
    char *current = data;
    while (current != data + dataLen) {
      bodyBuffer_ << *current;
      ++current;
    }
    return dataLen;
  }
  size_t onReadHeader(char *data, size_t size, size_t nMembers) override {
    size_t dataLen = size * nMembers;
    char *current = data;
    std::ostringstream headerBuff;
    while (current != data + dataLen) {
      headerBuff << *current;
      ++current;
    }
    rawHeaders_.push_back(headerBuff.str());
    return dataLen;
  }
  void onError(const std::exception &ex) override {
    LOG(INFO) << "ERROR ! " << ex.what();
  }
  void onFinished() override {
    LOG(INFO) << "onFinished!";
  }
};

int main() {
  google::InstallFailureSignalHandler();
  auto ebt = EBT::createShared();
  ebt->start();
  ebt->getBase()->runInEventBaseThread([ebt]() {
    auto curler = Curler::createPtr(ebt->getBase());
    curler->getUrl("http://localhost", new Handler);
  });
  ebt->join();
  LOG(INFO) << "end.";
}
