#include <fstream>
#include <iostream>

#include "cereal/archives/json.hpp"
#include "point.h"
#include "kdtree/kdtree.hpp"

using namespace std;

int main() {
	vector<Point> points = {
    {10, 0, {{"mac1", {100, 90}}}},
		{ 9, 1, {{"mac2", {110, 91}}}},
		{40, 0, {{"mac3", {120, 92}}}},
		{80, 0, {{"mac4", {130, 93}}}},
		{20, 1, {{"mac5", {140, 94}}}}
	};

  ofstream os("sample.json");
  cereal::JSONOutputArchive archive(os);

  archive(points);

  kdtree::kdtree<Point> * tree = new kdtree::kdtree<Point>(points);
  tree->k_nearest({10, 0}, 2);
}
