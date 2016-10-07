#include <thread>
#include <string>
#include <set>
#include <memory>
#include <glog/logging.h>
#include <folly/FBString.h>
#include <folly/Format.h>

using namespace std;

int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "begin";
  LOG(INFO) << "end.";
}
