#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

using namespace std;

struct PointEstimate {
  int x_mean;
  float x_var;
  int y_mean;
  float y_var;
};

#endif // COMMON_UTILS_H
