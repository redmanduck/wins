#include <cmath>
#include <fstream>

#include "cereal/archives/binary.hpp"
#include "map.h"

vector<Point> Map::likely_points_;
Graph Map::graph_;

void Map::InitMap(string filename) {
  ifstream is(filename, ios::binary);
  cereal::BinaryInputArchive archive(is);

  archive(graph_);
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
