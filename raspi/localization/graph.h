#ifndef GRAPH_H
#define GRAPH_H

#include <cassert>
#include <unordered_map>

#include "common_utils.h"
#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/vector.hpp"

#define GRAPH_VERSION 1

struct MacInfo {
  float mean;
  float std;

  template<class Archive>
  void serialize(Archive & archive) {
    archive(mean, std);
  }
};

struct Point {
  float x;
  float y;
  unordered_map<string, MacInfo> info;
  vector<Point> neighbors;

  template<class Archive>
  void serialize(Archive & archive) {
    archive(x, y, info, neighbors);
  }
  Point();
  Point(int,int);
};

Point::Point(){
	x = 0;
	y = 0;
}

Point::Point(int xv, int yv){
	x = xv;
	y = yv;
}

struct Graph {
  vector<Point> points;

  template<class Archive>
  void serialize(Archive & archive, uint32_t const version) {
    assert(version == GRAPH_VERSION);
    archive(points);
  }
};

CEREAL_CLASS_VERSION(Graph, GRAPH_VERSION);

#endif // GRAPH_H
