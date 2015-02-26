#include <cmath>
#include <fstream>
#include <limits>

#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/vector.hpp"
#include "map.h"

vector<kdtree::node<Point*>*> Map::likely_points_;
vector<unique_ptr<Point>> Map::all_points_;
unique_ptr<kdtree::kdtree<Point*>> Map::tree_;

void Map::InitMap(string filename) {
  ifstream is(filename, ios::binary);
  cereal::BinaryInputArchive archive(is);

  archive(all_points_);
  is.close();

  tree_.reset(new kdtree::kdtree<Point*>(&all_points_));
  Global::Init();
  likely_points_ = tree_->radius_nearest(all_points_[0].get(),
      numeric_limits<double>::max());
}

void Map::TryConvertJSONMap(string filename) {
  ifstream is(filename);
  cereal::JSONInputArchive in_archive(is);
  in_archive(all_points_);
  is.close();

  ofstream os(Global::MapFile);
  cereal::BinaryOutputArchive out_archive(os);
  out_archive(all_points_);
  os.close();
}

ProbabilityStat Map::Stats(const Point* p, string mac, int signal) {
  auto mac_info_iter = p->info.find(mac);
  if (mac_info_iter == p->info.end()) {
    // TODO: Handle error.
    return ProbabilityStat(-1, -1, -1);
  }
  auto&& mac_info = (*mac_info_iter).second;

  return ProbabilityStat(mac_info.mean, mac_info.var, signal);
}

const vector<kdtree::node<Point*>*>& Map::CurrentLikelyPoints() {
  return likely_points_;
}
