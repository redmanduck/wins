#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <string>
#include <vector>

using namespace std;
using result = tuple<double, double, double, double, double, double>;

struct DebugParams {
  double exp1;
  double exp2;
  double& mean;
  double& std;
  double& mean_var_x;
  double& mean_var_y;
};

void learn_helper(int argc, vector<string> argv);

#endif
