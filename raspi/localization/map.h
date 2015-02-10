#ifndef MAP_H
#define MAP_H

#include <unordered_map>
#include <vector>

#include "common_utils.h"
#include "probability_stat.h"

struct MacInfo {
  float mean;
  float std;
};

class PointInfo {
  
};

class Map {
 private:
  static vector<Point> likely_points_;
  static map<int, map<int, map<int, PointInfo<string, MacInfo>>>> map_;

 public:  
  static void InitMap();
  static void UpdateLikelyPoints(
  static ProbabilityStat Stats(Point p, string mac, int signal);
  static const vector<Point>& CurrentLikelyPoints();
};

#endif // MAP_H
