#include <stdexcept>

#include "location.h"

kdtree::node<Point*>* Location::current_node_ = nullptr;

kdtree::node<Point*>* Location::GetCurrentNode() {
  return current_node_;
}

void Location::UpdateEstimate() {
  throw runtime_error("Not Implemented");
}
