#include <fstream>
#include <iostream>

#include "cereal/archives/json.hpp"
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
    for (auto&& point : Map::CurrentLikelyPoints()) {
      for (auto&& scan : point.scans) {
        out_file << to_string(point.x) + "," + to_string(point.y) + ",";
        out_file << WifiEstimate::ClosestByMahalanobis(
            &scan, (Variant)(VARIANT_CHI_SQ | VARIANT_TOP1)).to_string();
        out_file << ",";
        out_file << WifiEstimate::ClosestByMahalanobis(
            &scan, (Variant)(VARIANT_CHI_SQ | VARIANT_TOP_FEW)).to_string();
        out_file << ",";
        out_file << WifiEstimate::ClosestByMahalanobis(
            &scan, VARIANT_CHI_SQ).to_string();
        out_file << ",";
        out_file << WifiEstimate::ClosestByMahalanobis(
            &scan, VARIANT_NONE).to_string();
        out_file << ",";
        PointEstimate p2 = WifiEstimate::MostProbableClubbed(scan);
        out_file << p2.to_string();
        out_file << ",";
        PointEstimate p3 = WifiEstimate::MostProbableNotClubbed(scan);
        out_file << p3.to_string();
        out_file << "\n";
      }
    }
    out_file.close();
  } else if (string(argv[1]) == "sample") {
      vector<Point> points = {
        {10, 0, {{"mac1", {100, 90}}, {"mac6", {150, 95}}},
          {{{"mac11", 60}, {"mac12", 70}},
           {{"mac11", 65}, {"mac12", 75}}}},
        { 9, 1, {{"mac2", {110, 91}}, {"mac7", {160, 96}}}},
        {40, 0, {{"mac3", {120, 92}}}},
        {80, 0, {{"mac4", {130, 93}}}},
        {20, 1, {{"mac5", {140, 94}}}}
      };

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
