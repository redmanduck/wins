#ifndef PROBABILITY_STAT_H
#define PROBABILITY_STAT_H

class ProbabilityStat {
 private:
  double mean_;
  double std_;
  double prob_;
  double dist_mean_;
  double x_;

 public:
  ProbabilityStat(double mean, double var, double x);
  double mean() { return mean_; }
  double std() { return std_; }
  double prob();
  double precision();
  double dist_mean();
};

#endif // PROBABILITY_STAT_H
