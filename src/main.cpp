#include <glog/logging.h>


int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start";
  LOG(INFO) << "end";
}