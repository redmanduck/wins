#include <cmath>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <sys/stat.h>

#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/vector.hpp"
#include "map.h"

mutex Map::navmode_mutex_;
NavMode Map::navmode_ = NAV_MODE_NONE;
condition_variable Map::navmode_changed_;
vector<kdtree::node<Point*>*> Map::likely_points_;
vector<unique_ptr<Point>> Map::all_points_;
unique_ptr<kdtree::kdtree<Point*>> Map::tree_;

inline bool file_exists(const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

void Map::SetNavMode(NavMode mode) {
  lock_guard<mutex> lock(navmode_mutex_);
  navmode_ = mode;
  navmode_changed_.notify_all();
}

bool Map::IsNavigating() {
  return navmode_ != NAV_MODE_NONE;
}

void Map::BlockUntilNavigating() {
  unique_lock<mutex> lock(navmode_mutex_);
  while (navmode_ == NAV_MODE_NONE) {
    navmode_changed_.wait(lock);
  }
}

void Map::InitMap(string filename) {
  ifstream is(filename, ios::binary);
  cereal::BinaryInputArchive archive(is);

  archive(all_points_);
  is.close();

  tree_.reset(new kdtree::kdtree<Point*>(&all_points_));
  likely_points_ = tree_->radius_nearest(all_points_[0].get(),
      numeric_limits<double>::max());
}

void Map::TryConvertJSONMap(string in_filename, string out_filename) {
  ifstream is(in_filename);
  if (not file_exists(in_filename)) {
    throw runtime_error("Map file does not exist");
  }
  cereal::JSONInputArchive in_archive(is);
  in_archive(all_points_);
  is.close();

  ofstream os(out_filename);
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

kdtree::node<Point*>* Map::NodeNearest(double x, double y) {
  Point p({ x, y });
  return tree_->nearest(&p);
}
