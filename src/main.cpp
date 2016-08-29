#include <glog/logging.h>
#include <thread>
#include <pthread.h>
#include <string>
// #include <spdlog/spdlog.h>

using namespace std;

int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start.";
  LOG(INFO) << "end.";
}
