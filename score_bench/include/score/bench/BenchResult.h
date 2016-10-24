#pragma once

#include <string>
#include <sstream>

namespace score { namespace bench {

class BenchResult {
 protected:
  std::string name_ {"Default"};
  double mean_ {0.0};
  double stdev_ {0.0};
  double min_ {0.0};
  double max_ {0.0};
 public:
  BenchResult();
  BenchResult(const std::string& name);
  BenchResult& setMean(double mean);
  BenchResult& setStdev(double stdev);
  BenchResult& setMin(double minVal);
  BenchResult& setMax(double maxVal);
  BenchResult& setName(const std::string& name);
  double getMin() const;
  double getMax() const;
  double getMean() const;
  double getStdev() const;
  const std::string& getName() const;
};

}} // score::bench

std::ostream& operator<<(std::ostream& oss, const score::bench::BenchResult& benched);
