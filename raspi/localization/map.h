#ifndef MAP_H
#define MAP_H

#include <unordered_map>

#include "common_utils.h"
#include "graph.h"
#include "probability_stat.h"

class Map {
 private:
  static vector<Point> likely_points_;
  static Graph graph_;

 public:  
  static void InitMap(string filename);
  static void UpdateLikelyPoints();
  static ProbabilityStat Stats(const Point& p, string mac, int signal);
  static const vector<Point>& CurrentLikelyPoints();
};

#endif // MAP_H
