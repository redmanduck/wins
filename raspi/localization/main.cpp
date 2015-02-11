#include <fstream>
#include <iostream>

#include "cereal/archives/json.hpp"
#include "graph.h"
#include "kdtree/kdtree.hpp"

using namespace std;

int main() {
	vector<Point> points = {
    {10, 0},
		{ 9, 1},
		{40, 0},
		{80, 0},
		{20, 1}
	};

  ofstream os("sample.json");
  cereal::JSONOutputArchive archive(os);

  archive(points);

  kdtree::kdtree<Point> * tree = new kdtree::kdtree<Point>(points);
  tree->k_nearest({10, 0}, 2);
}
