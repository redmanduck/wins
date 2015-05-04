#ifndef POINT_H
#define POINT_H

#include <cassert>
#include <list>
#include <unordered_map>

#include "common_utils.h"
#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/list.hpp"
#include "scan_result.h"

namespace wins {

#define POINT_VERSION 2

struct MacInfo {
  double mean;
  double var;

  template<class Archive>
  void serialize(Archive & archive) {
    archive(mean, var);
  }
};

struct Point {
  double x;
  double y;
  vector<int> cost;
  unordered_map<string, MacInfo> info;
  vector<vector<Result>> scans;
  double scale_x;
  double scale_y;

  template<class Archive>
  void serialize(Archive & archive, uint32_t const version) {
    assert(version == POINT_VERSION);
#ifdef DSCAN
    archive(x, y, cost, info, scans, scale_x, scale_y);
#else
    archive(x, y, cost, info, scale_x, scale_y);
#endif
  }
};

}

CEREAL_CLASS_VERSION(wins::Point, POINT_VERSION);

#endif // POINT_H
