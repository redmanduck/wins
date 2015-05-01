#include <sstream>

#include "common_utils.h"

namespace wins {

using namespace std;

double mean(vector<double> v) {
  double sum = std::accumulate(v.begin(), v.end(), 0.0);
  double mean = sum / v.size();
  return mean;
}

double stddev(vector<double> v) {
  std::vector<double> diff(v.size());
  std::transform(v.begin(), v.end(), diff.begin(),
                 std::bind2nd(std::minus<double>(), mean(v)));
  double sq_sum = std::inner_product(diff.begin(), diff.end(),
      diff.begin(), 0.0);
  double stdev = std::sqrt(sq_sum / v.size());
  return stdev;
}

std::vector<std::string> &split(const std::string &s,
  char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

}
