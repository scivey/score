#include <glog/logging.h>
#include <thread>
#include <pthread.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <sys/select.h>


#include <spdlog/spdlog.h>
#include <fcntl.h>
#include <curl/curl.h>

#include "score/reactor/ReactorThread.h"
#include "score/macros.h"
#include "score/reactor/TimerFd.h"
#include "score/locks/ThreadBaton.h"

using namespace std;
using namespace score::locks;
using namespace score::reactor;




size_t write_callback(char *ptr, size_t sz, size_t nmemb, void *userdata) {
  LOG(INFO) << "write_callback [" << sz << ", " << nmemb << "]";
  return nmemb;
}

size_t header_callback(char *ptr, size_t sz, size_t nmemb, void *userdata) {
  LOG(INFO) << "header_callback [" << sz << ", " << nmemb << "]";
  return nmemb;
}

class TickHandler : public TimerFd::EventHandler {
 protected:
  std::atomic<size_t> &counter_;
  ThreadBaton &baton_;
 public:
  TickHandler(std::atomic<size_t> &cnt, ThreadBaton &bat)
    : counter_(cnt), baton_(bat) {}

  void onTick() override {
    if (counter_.fetch_add(1) == 5) {
      stop();
      baton_.post();
    }
  }
};


// int main() {
  // google::InstallFailureSignalHandler();
  // auto console = spdlog::stdout_logger_mt("main", true);
  // auto rt = ReactorThread::createShared();
  // rt->start();
  // ThreadBaton bat1, bat2;
  // std::atomic<size_t> counter {0};
  // rt->runInEventThread([&bat1, &counter, rt]() {
  //   TimerSettings settings {
  //     chrono::milliseconds {1000},
  //     chrono::milliseconds {1000}
  //   };
  //   auto tm = TimerFd::Factory::createRaw(settings, new TickHandler(
  //     counter, bat1
  //   ));
  //   rt->addTask(tm->getEpollTask(), []() {
  //     LOG(INFO) << "task added.";
  //   });
  // });
  // bat1.wait();
  // // auto curl = curl_easy_init();
  // // CHECK(curl);
  // // CURLcode res;
  // // console->info("requesting...");
  // // curl_easy_setopt(curl, CURLOPT_URL, "http://jezebel.com/rss");
  // // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  // // curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
  // // res = curl_easy_perform(curl);
  // // console->info("got result.");
  // // curl_easy_cleanup(curl);
  // LOG(INFO) << "end.";
// }

int main() {
  google::InstallFailureSignalHandler();
  // auto console = spdlog::stdout_logger_mt("main", true);
  // auto rt = ReactorThread::createShared();
  // rt->start();
  // ThreadBaton bat1, bat2;
  // std::atomic<size_t> counter {0};
  // rt->runInEventThread([&bat1, &counter, rt]() {
  //   TimerSettings settings {
  //     chrono::milliseconds {1000},
  //     chrono::milliseconds {1000}
  //   };
  //   auto tm = TimerFd::Factory::createRaw(settings, new TickHandler(
  //     counter, bat1
  //   ));
  //   rt->addTask(tm->getEpollTask(), []() {
  //     LOG(INFO) << "task added.";
  //   });
  // });
  // bat1.wait();

  fd_set fdread;
  fd_set fdwrite;
  fd_set fdexcept;
  int maxfd {-1};
  long curlTimeout {0};
  auto curlm = curl_multi_init();
  curl_multi_timeout(curlm, &curlTimeout);
  if (curlTimeout < 0) {
    curlTimeout = 1000;
  }
  struct timeval timeout;
  timeout.tv_sec = curlTimeout / 1000;
  timeout.tv_usec = (curlTimeout % 1000) * 1000;
  FD_ZERO(&fdread);
  FD_ZERO(&fdwrite);
  FD_ZERO(&fdexcept);
  int stillRunning {10};

  auto curl1 = curl_easy_init();
  curl_easy_setopt(curl1, CURLOPT_URL, "http://jezebel.com/rss");
  curl_easy_setopt(curl1, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl1, CURLOPT_HEADERFUNCTION, header_callback);

  // res = curl_easy_perform(curl1);
  curl_multi_add_handle(curlm, curl1);
  int repeats = 0;
  do {
    CURLMcode mc;
    int numfds;
    mc = curl_multi_perform(curlm, &stillRunning);
    if (mc == CURLM_OK) {
      mc = curl_multi_wait(curlm, nullptr, 0, 1000, &numfds);
    }
    if (mc != CURLM_OK) {
      fprintf(stderr, "curl_muilti failed, code %d.n", mc);
      break;
    }
    if (!numfds) {
      repeats++;
      if (repeats > 1) {
        this_thread::sleep_for(chrono::milliseconds(100));
      }
    } else {
      repeats = 0;
    }
  } while (stillRunning);

  // curl_easy_cleanup(curl);
  curl_multi_cleanup(curlm);
  LOG(INFO) << "end.";
}
