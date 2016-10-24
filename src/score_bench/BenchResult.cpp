#include "score_bench/BenchResult.h"

using namespace std;

namespace score { namespace bench {

BenchResult::BenchResult(){}

BenchResult::BenchResult(const std::string& name)
  : name_(name) {}

BenchResult& BenchResult::setMean(double mean) {
  mean_ = mean;
  return *this;
}

BenchResult& BenchResult::setStdev(double stdev) {
  stdev_ = stdev;
  return *this;
}

BenchResult& BenchResult::setMin(double minVal) {
  min_ = minVal;
  return *this;
}

BenchResult& BenchResult::setMax(double maxVal) {
  max_ = maxVal;
  return *this;
}

BenchResult& BenchResult::setName(const std::string& name) {
  name_ = name;
  return *this;
}

double BenchResult::getMin() const {
  return min_;
}

double BenchResult::getMax() const {
  return max_;
}

double BenchResult::getMean() const {
  return mean_;
}

double BenchResult::getStdev() const {
  return stdev_;
}

const std::string& BenchResult::getName() const {
  return name_;
}

}} // score::bench


std::ostream& operator<<(std::ostream& oss, const score::bench::BenchResult& benched) {
  oss << "\n\t[" << benched.getName() << "]" << endl;
  oss << "\t\tmean:  " << benched.getMean() << endl
      << "\t\tstdev: " << benched.getStdev() << endl
      << "\t\tmin:   " << benched.getMin() << endl
      << "\t\tmax:   " << benched.getMax() << endl
      << endl;
  return oss;
}
