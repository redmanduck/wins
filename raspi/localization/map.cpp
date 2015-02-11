#include <cmath>
#include <fstream>

#include "cereal/archives/binary.hpp"
#include "map.h"

vector<Point> Map::likely_points_;
unique_ptr<kdtree::kdtree<Point>> Map::tree_;

void Map::InitMap(string filename) {
  vector<Point> points;

  ifstream is(filename, ios::binary);
  cereal::BinaryInputArchive archive(is);

  archive(points);

  tree_.reset(new kdtree::kdtree<Point>(points));
}

void Map::TryLoadRawMap(string filename) {

}

ProbabilityStat Map::Stats(const Point& p, string mac, int signal) {
  auto mac_info_iter = p.info.find(mac);
  if (mac_info_iter == p.info.end()) {
    // TODO: Handle error.
    return ProbabilityStat(-1, -1, -1);
  }
  auto&& mac_info = (*mac_info_iter).second;
  
  return ProbabilityStat(mac_info.mean, mac_info.std, signal);
}

const vector<Point>& Map::CurrentLikelyPoints() {
  return likely_points_;
}
