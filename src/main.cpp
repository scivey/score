#include <glog/logging.h>
#include <string>
#include <sstream>
#include "score/func/Function.h"
#include "score/func/Callback.h"
#include "score/func/callable_traits.h"
#include "score/demangle.h"

using namespace std;

uint64_t add2(uint32_t x, uint16_t y) {
  uint64_t result = x;
  result += y;
  return result;
}

std::string joinStr(string str1, string str2) {
  std::ostringstream oss;
  oss << str1 << " || " << str2;
  return oss.str();
}
namespace func = score::func;
using ftraits = func::callable_traits<decltype(add2)>;


int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start";
  using rt = typename ftraits::result_type;
  using fst = typename ftraits::nth_arg_type<0>;
  using snd = typename ftraits::nth_arg_type<1>;
  LOG(INFO) << score::demangle(typeid(rt));
  LOG(INFO) << score::demangle(typeid(fst));
  LOG(INFO) << score::demangle(typeid(snd));
  LOG(INFO) << ftraits::arg_count;
}
