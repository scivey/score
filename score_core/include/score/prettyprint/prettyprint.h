#pragma once
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
#include "score/prettyprint/prettyprint_predef.h"

namespace score { namespace prettyprint {

template<>
struct PrettyPrinter<std::string> {
  static void pprint(std::ostream &oss, const std::string &ref) {
    oss << "'" << ref << "'";
  }
};

template<typename T>
struct PrettyPrinter<std::vector<T>> {
  static void pprint(std::ostream &oss, const std::vector<T> &ref) {
    oss << "vector([ ";
    if (!ref.empty()) {
      size_t last = ref.size() - 1;
      for (size_t i = 0; i <= last; i++) {
        prettyPrint(oss, ref[i]);
        if (i != last) {
          oss << ", ";
        }
      }
    }
    oss << " ])";
  }
};

template<typename T1, typename T2>
struct PrettyPrinter<std::map<T1, T2>> {
  static void pprint(std::ostream &oss, const std::map<T1, T2> &ref) {
    oss << "map({ ";
    if (!ref.empty()) {
      size_t last = ref.size() - 1;
      size_t i = 0;
      for (auto &item: ref) {
        prettyPrint(oss, item.first);
        oss << " : ";
        prettyPrint(oss, item.second);
        if (i != last) {
          oss << ", ";
        }
        i++;
      }
    }
    oss << " })";
  }
};

template<typename T1, typename T2>
struct PrettyPrinter<std::unordered_map<T1, T2>> {
  static void pprint(std::ostream &oss, const std::unordered_map<T1, T2> &ref) {
    oss << "unordered_map({ ";
    if (!ref.empty()) {
      size_t last = ref.size() - 1;
      size_t i = 0;
      for (auto &item: ref) {
        prettyPrint(oss, item.first);
        oss << " : ";
        prettyPrint(oss, item.second);
        if (i != last) {
          oss << ", ";
        }
        i++;
      }
    }
    oss << " })";
  }
};

}} // score::prettyprint

