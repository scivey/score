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
#include <unicode/brkiter.h>

using namespace std;

void attempt(const UnicodeString& str) {
  UErrorCode status = U_ZERO_ERROR;
  BreakIterator* bi = BreakIterator::createWordInstance(
    Locale::getUS(), status
  );
  bi->setText(str);
  int32_t p = bi->first();
  while (p != BreakIterator::DONE) {
    LOG(INFO) << "boundary at : " << p;
    p = bi->next();
  }
  delete bi;

}

int main() {
  google::InstallFailureSignalHandler();
  UnicodeString someStr = "this is a test";
  attempt(someStr);
  LOG(INFO) << "end.";
}
