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
#include "score/iter/IterTrailer.h"

#include <unicode/brkiter.h>

using namespace score::iter;
using namespace std;

int main() {
  google::InstallFailureSignalHandler();
  string someStr = "this is a test";
  std::vector<int> something {5, 7, 3, 9, 10};
  auto trailer = iterTrailing(something);
  for (auto elem: trailer) {
    LOG(INFO) << elem.first << "\t" << elem.second;
  }
  LOG(INFO) << "end.";
}
