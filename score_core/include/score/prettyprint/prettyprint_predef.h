#pragma once
#include <string>
#include <sstream>

namespace score { namespace prettyprint {

template<typename T>
struct PrettyPrinter {
  static void pprint(std::ostream &oss, const T& ref) {
    oss << ref;
  }
};

template<typename T>
void prettyPrint(std::ostream &oss, const T& ref) {
  PrettyPrinter<T>::pprint(oss, ref);
}

template<typename T>
std::string prettyPrint(const T& ref) {
  std::ostringstream oss;
  prettyPrint(oss, ref);
  return oss.str();
}

}} // score::prettyprint
