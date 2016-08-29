#include <glog/logging.h>
#include <thread>
#include <pthread.h>
#include <string>
#include "aliens/vendored/seastar/core/shared_ptr.hh"

using namespace std;

int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start.";
  LOG(INFO) << "end.";
}
