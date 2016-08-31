#include <glog/logging.h>
#include <thread>
#include <pthread.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "aliens/vendored/seastar/core/shared_ptr.hh"
#include <openssl/md5.h>
#include <openssl/sha.h>

using namespace std;

string getTestStr() {
  ostringstream oss;
  for (size_t i = 0; i < 100; i ++) {
    oss << "testing\t";
  }
  return oss.str();
}

int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start.";
  auto something = getTestStr();
  auto resultPtr = (uint8_t*) malloc(SHA_DIGEST_LENGTH);
  SHA1((uint8_t*) something.c_str(), something.size(), resultPtr);
  // ostringstream oss;
  LOG(INFO) << "shadig : " << SHA_DIGEST_LENGTH;
  auto foo = (const char*) resultPtr;
  LOG(INFO) << std::hex << foo;
  // for (size_t i = 0; i < MD5_DIGEST_LENGTH; i++) {
  //   uint8_t* ptr = resultPtr + i;
  //   unsigned char cc = *ptr;
  //   LOG(INFO) << cc;
  //   oss << cc;
  // }
  // LOG(INFO) << std::hex << oss.str();
  LOG(INFO) << "end.";
}
