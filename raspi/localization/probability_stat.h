#ifndef PROBABILITY_STAT_H
#define PROBABILITY_STAT_H

class ProbabilityStat {
 private:
  float mean_;
  float std_;
  float prob_;
  float dist_mean_;
  float x_;

 public:
  ProbabilityStat(float mean, float std, float x);
  float mean() { return mean_; }
  float std() { return std_; }
  float prob();
  float precision();
  float dist_mean();
};

#endif // PROBABILITY_STAT_H
