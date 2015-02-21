#ifndef MAP_H
#define MAP_H

#include <memory>
#include <unordered_map>

#include "common_utils.h"
#include "point.h"
#include "kdtree/kdtree.hpp"
#include "probability_stat.h"

class Map {
 private:
  static vector<kdtree::node<Point*>*> likely_points_;
  static vector<unique_ptr<Point>> all_points_;
  static unique_ptr<kdtree::kdtree<Point*>> tree_;

 public:
  static void InitMap(string filename);
  static void TryConvertJSONMap(string filename);
  static void UpdateLikelyPoints();
  static ProbabilityStat Stats(const Point* p, string mac, int signal);
  static const vector<kdtree::node<Point*>*>& CurrentLikelyPoints();
  static const vector<unique_ptr<Point>>& all_points() {
    return all_points_;
  }
};

#endif // MAP_H
