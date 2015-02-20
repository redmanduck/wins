#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <cstdlib>
#include <string>
#include <utility>
#include <vector>
#include <list>

using namespace std;

#define DSCAN

struct PointEstimate {
  int x_mean;
  float x_var;
  int y_mean;
  float y_var;
  string to_string() {
    return std::to_string(x_mean) + "," +
           std::to_string(x_var) + "," +
           std::to_string(y_mean) + "," +
           std::to_string(y_var);
  }
};

class Global {
 public:
  static string MapFile;
};

#endif // COMMON_UTILS_H
