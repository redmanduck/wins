#include <cmath>
#include <fstream>

#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"
#include "map.h"

vector<Point> Map::likely_points_;
unique_ptr<kdtree::kdtree<Point>> Map::tree_;

void Map::InitMap(string filename) {
  vector<Point> points;

  ifstream is(filename, ios::binary);
  cereal::BinaryInputArchive archive(is);

  archive(points);
  is.close();

  tree_.reset(new kdtree::kdtree<Point>(points));
  likely_points_ = points;
}

void Map::TryConvertJSONMap(string filename) {
  vector<Point> points;

  ifstream is(filename);
  cereal::JSONInputArchive in_archive(is);
  in_archive(points);
  is.close();

  ofstream os(Global::MapFile);
  cereal::BinaryOutputArchive out_archive(os);
  out_archive(points);
  os.close();
}

ProbabilityStat Map::Stats(const Point& p, string mac, int signal) {
  auto mac_info_iter = p.info.find(mac);
  if (mac_info_iter == p.info.end()) {
    // TODO: Handle error.
    return ProbabilityStat(-1, -1, -1);
  }
  auto&& mac_info = (*mac_info_iter).second;
  
  return ProbabilityStat(mac_info.mean, mac_info.var, signal);
}

const vector<Point>& Map::CurrentLikelyPoints() {
  return likely_points_;
}
