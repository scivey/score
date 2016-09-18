#include <glog/logging.h>
#include <thread>




using namespace std;

int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "begin";
  LOG(INFO) << "end.";
}
