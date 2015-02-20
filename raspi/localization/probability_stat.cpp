#include <cmath>
#include <cstdlib>

#include "gamma.hpp"
#include "probability_stat.h"

namespace {
  float StandardDistribution(float x) {
    return exp(- x * x / 2) / sqrt(2 * M_PI);
  }
} // anonymous namespace

ProbabilityStat::ProbabilityStat(float m, float v, float x) {
  mean_ = m;
  std_ = sqrt(v);
  prob_ = -1;
  dist_mean_ = -1;
  x_ = x;
}

float ProbabilityStat::precision() {
  return 1 / (std_ * std_);
}

float ProbabilityStat::prob() {
  if(prob_ == -1) {
    prob_ = dnorm(x_, mean_, std_);
  }
  return prob_;
}

float ProbabilityStat::dist_mean() {
  return fabs((x_ - mean_) / std_);
}
