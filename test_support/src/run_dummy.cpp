#include <glog/logging.h>

int main(int argc, char *argv[]) {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "This doesn't do anything.";
}

