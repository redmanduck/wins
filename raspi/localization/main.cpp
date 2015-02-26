#include <fstream>
#include <iostream>
#include <memory>

#include "cereal/archives/json.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/vector.hpp"
#include "kdtree/kdtree.hpp"
#include "map.h"
#include "point.h"
#include "gamma.hpp"
#include "wifi_estimate.h"

#include <cassert>

using namespace std;

bool DoDebugRun(int argc, char *argv[]) {
  if (argc == 1)
    return false;

  if (string(argv[1]) == "make_binary") {
    assert(argc == 3);
    Map::TryConvertJSONMap(argv[2]);
    return true;
  } else if (string(argv[1]) == "test_data") {
    Map::InitMap(Global::MapFile);
    ofstream out_file;
    out_file.open("results.csv");
    for (auto&& point : Map::all_points()) {
      cout << to_string(point->x) + "," + to_string(point->y) + "\n";
      for (auto&& scan : point->scans) {
        out_file << to_string(point->x) + "," + to_string(point->y) + ",";
        for (auto&& estimate : WifiEstimate::ClosestByMahalanobis(
            &scan, (WiFiVariant)(WIFI_VARIANT_CHI_SQ | WIFI_VARIANT_TOP_FEW))) {
          out_file << estimate.to_string();
          out_file << ",";
        }
        /*
        out_file << WifiEstimate::ClosestByMahalanobis(
            &scan, WIFI_VARIANT_CHI_SQ).to_string();
        out_file << ",";
        out_file << WifiEstimate::ClosestByMahalanobis(
            &scan, WIFI_VARIANT_NONE).to_string();
        out_file << ",";
        PointEstimate p2 = WifiEstimate::MostProbableClubbed(scan);
        out_file << p2.to_string();
        out_file << ",";
        PointEstimate p3 = WifiEstimate::MostProbableNotClubbed(scan);
        out_file << p3.to_string();
        */
        out_file << "\n";
      }
    }
    out_file.close();
  } else if (string(argv[1]) == "sample") {
      unique_ptr<Point> p = unique_ptr<Point>(new Point {20, 1, {{"mac5", {140, 94}}}});
      vector<unique_ptr<Point>> points;
        points.emplace_back(unique_ptr<Point>(new Point {10, 0,
          {{"mac1", {100, 90}}, {"mac6", {150, 95}}},
          {{{"mac11", 60}, {"mac12", 70}},
           {{"mac11", 65}, {"mac12", 75}}}}));
        points.emplace_back(unique_ptr<Point>(new Point { 9, 1,
          {{"mac2", {110, 91}}, {"mac7", {160, 96}}}}));

      ofstream os("sample.json");
      cereal::JSONOutputArchive archive(os);

      archive(points);
  } else if (string(argv[1]) == "test_math") {
    cout << "chi(2, 15) = " << pchisq(2, 15) << "\n";
    cout << "chi(15, 15) = " << pchisq(15, 15) << "\n";
    cout << "chi(15, 2) = " << pchisq(15, 2) << "\n";
    cout << "dnorm(1, 1, 1) = " << dnorm(0, 0, 1) << "\n";
    cout << "pnorm1(1, 0, 0) = " << pnorm1(0, false, false) << "\n";
    cout << "pnorm1(1, 0, 0) = " << pnorm1(0, false, true) << "\n";
    cout << "pnorm1(1, 0, 0) = " << pnorm1(0, true, true) << "\n";
    cout << "pnorm1(1, 0, 0) = " << pnorm1(0, true, true) << "\n";
  }
  return false;
}

void RunMainLoop(int argc, char *argv[]) {

}

int main(int argc, char *argv[]) {
  if (!DoDebugRun(argc, argv)) {
    RunMainLoop(argc, argv);
  }
  return 0;
}
