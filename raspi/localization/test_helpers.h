#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <fstream>
#include <string>
#include <vector>

class FakeWifiScan;

namespace wins {

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
int AddNextSet(ifstream& fs, FakeWifiScan* fakescanner);

}

#endif
