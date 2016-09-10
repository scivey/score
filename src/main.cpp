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
#include <folly/Format.h>

#include <spdlog/spdlog.h>

#include "score/macros.h"
#include "score/curl/RawCurlResponse.h"
#include "score/curl/SimpleCurler.h"
#include "score/curl/CurlError.h"

#include "score/folly_util/EBThread.h"

#include <folly/Optional.h>
#include <folly/ExceptionWrapper.h>


using folly::Optional;
using folly::exception_wrapper;

using namespace score;
using namespace std;
using namespace score::curl;
using EBT = score::folly_util::EBThread<folly::EventBase>;

int main() {
  google::InstallFailureSignalHandler();
  auto ebt = EBT::createShared();
  ebt->start();
  ebt->getBase()->runInEventBaseThread([ebt]() {
    auto curler = SimpleCurler::createPtr(ebt->getBase());
    curler->getUrl("http://localhost", [](Optional<exception_wrapper> err, Optional<RawCurlResponse> res) {
      LOG(INFO) << "done.";
      LOG(INFO) << folly::format("has error? {}   has res? {}",
        err.hasValue(), res.hasValue()
      );
      if (res.hasValue()) {
        LOG(INFO) << res.value().bodyBuffer.str();
      }
    });
  });
  ebt->join();
  LOG(INFO) << "end.";
}
