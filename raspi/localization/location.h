#ifndef LOCATION_H
#define LOCATION_H

#include "common_utils.h"
#include "kdtree/node.hpp"
#include "point.h"

class Location {
 private:
  static kdtree::node<Point>* current_node_;
  static PointEstimate point_estimate_;

 public:
  static kdtree::node<Point>* GetCurrentNode();
  static void UpdateEstimate();
};

#endif
