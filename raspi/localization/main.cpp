#include <iostream>
#include "kdtree/kdtree.hpp"
#include "graph.h"

using namespace std;

int main() {
	vector<Point> points = {
		Point(10, 0),
		Point(9, 1),
		Point(40, 0),
		Point(80, 0),
		Point(20, 1)
	};

  kdtree::kdtree<Point> * tree = new kdtree::kdtree<Point>(points);
  tree->k_nearest(Point(10, 0), 2);
}
