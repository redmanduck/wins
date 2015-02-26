#include <cmath>
#include <cstdlib>

#include "gamma.hpp"
#include "probability_stat.h"

namespace {
  double StandardDistribution(double x) {
    return exp(- x * x / 2) / sqrt(2 * M_PI);
  }
} // anonymous namespace

ProbabilityStat::ProbabilityStat(double m, double v, double x) {
  mean_ = m;
  std_ = sqrt(v);
  prob_ = -1;
  dist_mean_ = -1;
  x_ = x;
}

double ProbabilityStat::precision() {
  return 1 / (std_ * std_);
}

double ProbabilityStat::prob() {
  if(prob_ == -1) {
    prob_ = dnorm(x_, mean_, std_);
  }
  return prob_;
}

double ProbabilityStat::dist_mean() {
  return fabs((x_ - mean_) / std_);
}
