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

#define POINT_VERSION 2

struct MacInfo {
  float mean;
  float var;

  template<class Archive>
  void serialize(Archive & archive) {
    archive(mean, var);
  }
};

struct Point {
  float x;
  float y;
  unordered_map<string, MacInfo> info;

  list<list<Result>> scans;

  template<class Archive>
  void serialize(Archive & archive, uint32_t const version) {
    assert(version == POINT_VERSION);
#ifdef DSCAN
    archive(x, y, info, scans);
#else
    archive(x, y, info);
#endif
  }
};

CEREAL_CLASS_VERSION(Point, POINT_VERSION);

#endif // POINT_H
