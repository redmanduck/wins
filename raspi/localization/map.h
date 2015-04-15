#ifndef MAP_H
#define MAP_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "common_utils.h"
#include "point.h"
#include "kdtree/kdtree.hpp"
#include "probability_stat.h"

namespace wins {

enum NavMode {
  NAV_MODE_NONE,
  NAV_MODE_ROUTE,
  NAV_MODE_LOCATE
};

class Map {
 private:
  static mutex navmode_mutex_;
  static NavMode navmode_;
  static condition_variable navmode_changed_;
  static vector<kdtree::node<Point*>*> likely_points_;
  static vector<unique_ptr<Point>> all_points_;
  static unique_ptr<kdtree::kdtree<Point*>> tree_;
  static thread navigation_thread_;
  static bool terminate_;

  static void MainLoop();

 public:
  static void StartNavigationThread();
  static void TerminateThread();

  static void SetNavMode(NavMode mode);
  static bool IsNavigating();
  static void BlockUntilNavigating();
  static void InitMap(string filename);
  static void TestInitMap(vector<unique_ptr<Point>>&& all_points);
  static void TryConvertJSONMap(string in_filename, string out_filename);
  static void UpdateLikelyPoints(double radius);
  static vector<kdtree::node<Point*>*> NodesInRadius(
      kdtree::node<Point*>* node, const double radius);
  static ProbabilityStat Stats(const Point* p, string mac, int signal);
  static const vector<kdtree::node<Point*>*>& CurrentLikelyPoints();
  static const vector<unique_ptr<Point>>& all_points() {
    return all_points_;
  }
  static kdtree::node<Point*>* NodeNearest(double x, double y);
};

}

#endif // MAP_H
