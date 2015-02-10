#include <cmath>

#include "map.h"

vector<Point> Map::likely_points_;
unordered_map<Point, unordered_map<string, MacInfo>> Map::map_;

ProbabilityStat Map::Stats(Point p, string mac, int signal) {
  auto point_stats_iter = map_.find(p);
  if (point_stats_iter == map_.end()) {
    // TODO: Handle error.
  }
  auto&& point_stats = (*point_stats_iter).second;
  auto mac_info_iter = point_stats.find(mac);
  if (mac_info_iter == point_stats.end()) {
    // TODO: Handle error.
    return ProbabilityStat(-1, -1, -1);
  }
  auto&& mac_info = (*mac_info_iter).second;
  
  return ProbabilityStat(mac_info.mean, mac_info.std, signal);
}

const vector<Point>& Map::CurrentLikelyPoints() {
  return likely_points_;
}
