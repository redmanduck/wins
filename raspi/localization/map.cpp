#include <cmath>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <sys/stat.h>

#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/vector.hpp"
#include "global.h"
#include "imu.h"
#include "location.h"
#include "navigation.h"
#include "map.h"

namespace wins {

mutex Map::navmode_mutex_;
NavMode Map::navmode_ = NAV_MODE_NONE;
condition_variable Map::navmode_changed_;
vector<kdtree::node<Point*>*> Map::likely_points_;
vector<unique_ptr<Point>> Map::all_points_;
unique_ptr<kdtree::kdtree<Point*>> Map::tree_;

thread Map::navigation_thread_;
bool Map::terminate_ = false;

inline bool file_exists(const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

void Map::MainLoop() {
  Map::InitMap(Global::MapFile);
	Location::Init();
  while(not terminate_) {
    Location::UpdateEstimate();
    Navigation::UpdateRoute();

    // Mark point after any route changes.
    Global::SetEventFlag(WINS_EVENT_POS_CHANGE);
  }
}

void Map::StartNavigationThread() {
  if (not navigation_thread_.joinable()) {
    navigation_thread_ = thread(&Map::MainLoop);
  }
}

void Map::TerminateThread() {
  terminate_ = true;
  navigation_thread_.join();
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

  //for(auto& p : all_points_){
  //  cout << p->scale_x << "," << p->scale_y << "\n";
  //}

  tree_.reset(new kdtree::kdtree<Point*>(&all_points_));
  likely_points_ = tree_->radius_nearest(all_points_[0].get(),
      numeric_limits<double>::max());
}

void Map::TestInitMap(vector<unique_ptr<Point>>&& all_points) {
  all_points_ = move(all_points);
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

void Map::UpdateLikelyPoints(double radius) {
  auto current = Location::GetCurrentNode();
  auto from = current == nullptr ? all_points_[0].get() : current->point;
  likely_points_ = tree_->radius_nearest(from, radius);
  //printf("-------\n%3s %3s\n", "X", "Y");
  //if (current != nullptr) {
  //  printf("%3f %3f\n", current->point->x, current->point->y);
  //}
  //printf("-------\n");
  //for (auto p : likely_points_) {
  //  printf("%3f %3f\n", p->point->x, p->point->y);
  //}
  //printf("-------\n");
}

vector<kdtree::node<Point*>*> Map::NodesInRadius(kdtree::node<Point*>* node,
    const double radius) {
  return tree_->radius_nearest(node, radius);
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

}
