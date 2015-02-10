#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

struct Point;


using namespace std;

struct Point {
  int x;
  int y;

  bool operator==(const Point& other) const;
};

struct PointEstimate {
  int x_mean;
  float x_var;
  int y_mean;
  float y_var;
};

namespace std {

  template <>
  struct hash<Point>
  {
    std::size_t operator()(const Point& p) const {
      return std::hash<long>()((((long)p.x) << sizeof(int)) + p.y);
    }
  };
}

#endif // COMMON_UTILS_H
