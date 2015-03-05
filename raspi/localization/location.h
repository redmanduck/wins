#ifndef LOCATION_H
#define LOCATION_H

#include "common_utils.h"

class Location {
 private:
  Node<Point>* current_node_;
  PointEstimate point_estimate_;
};
