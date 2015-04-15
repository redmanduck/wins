#include "test.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>

#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/vector.hpp"
#include "common_utils.h"
#include "display.h"
#include "global.h"
#include "location.h"
#include "kdtree/kdtree.hpp"
#include "keypad_handler.h"
#include "test_helpers.h"
#include "map.h"
#include "navigation.h"
#include "point.h"
#include "scan_result.h"
#include "gamma.hpp"
#include "wifi_estimate.h"

#include <cassert>

namespace wins {

void Test(int argc, char *orig_argv[]) {
  vector<string> argv(orig_argv, orig_argv + argc);

  Global::SetTestMode();

  if (string(argv[2]) == "make_binary") {
    assert(argc == 5);
    Map::TryConvertJSONMap(argv[3], argv[4]);
  } else if (string(argv[2]) == "data") {
    assert(argc == 5);
    Map::InitMap(argv[3]);
    ofstream out_file;
    out_file.open("results.csv");

    vector<unique_ptr<Point>> test_points;
    ifstream is(argv[4], ios::binary);
    cereal::BinaryInputArchive archive(is);

    archive(test_points);
    is.close();

    auto w = WiFiEstimate();
    int count = 0;
    for (auto&& point : test_points) {
      printf("Ground Truth: %2.0f, %2.0f\n", point->x, point->y);
      for (auto&& scan : point->scans) {
        out_file << to_string(point->x) + "," + to_string(point->y) + ",";
        count += 1;
        if (count == 15) {
          cout <<15;
        }
        for (auto&& estimate : w.ClosestByMahalanobis(
            scan, (WiFiVariant)(WIFI_VARIANT_TOP1),
            point->x, point->y)) {
        //for (auto&& estimate : WiFiEstimate::MostProbableNotClubbed(scan)) {
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
  }
  else if (string(argv[2]) == "sample") {
      vector<unique_ptr<Point>> points;
      points.emplace_back(unique_ptr<Point>(new Point {10, 0, {1,1,1,1},
        {{"mac11", {100, 90}}, {"mac12", {150, 95}}},
        {{{"mac11", 60.0}, {"mac12", 70.0}},
         {{"mac11", 65.0}, {"mac12", 75.0}}}}));
      points.emplace_back(unique_ptr<Point>(new Point { 9, 1, {1,1,1,1},
        {{"mac11", {110, 91}}, {"mac12", {160, 96}}},
        {{{"mac11", 60.0}, {"mac12", 70.0}},
         {{"mac11", 65.0}, {"mac12", 75.0}}}}));

      ofstream os("sample.json");
      cereal::JSONOutputArchive archive(os);

      archive(points);
  }
  else if (string(argv[2]) == "math") {
    cout << "chi(2, 15) = " << pchisq(2, 15) << "\n";
    cout << "chi(15, 15) = " << pchisq(15, 15) << "\n";
    cout << "chi(15, 2) = " << pchisq(15, 2) << "\n";
    cout << "dnorm(1, 1, 1) = " << dnorm(0, 0, 1) << "\n";
    cout << "pnorm1(1, 0, 0) = " << pnorm1(0, false, false) << "\n";
    cout << "pnorm1(1, 0, 0) = " << pnorm1(0, false, true) << "\n";
    cout << "pnorm1(1, 0, 0) = " << pnorm1(0, true, true) << "\n";
    cout << "pnorm1(1, 0, 0) = " << pnorm1(0, true, true) << "\n";
  }
  else if (string(argv[2]) == "wifi") {
    std::vector<int> EEchan = { 1, 6, 11, 48, 149, 36, 40, 157, 44, 153,161};

    WifiScan w(EEchan, "wlp2s0");
    vector<Result> results = w.Fetch();

    for(auto& kv : results) {
      std::cout << kv.name << " = " << kv.signal << std::endl;
    }
  }
  else if (string(argv[2]) == "learn") {
    learn_helper(argc, argv);
  }
  else if (string(argv[2]) == "learn_all") {
    argc += 1;
    argv.push_back("-");
    remove("analysis_summary.csv");
    for (int i = 0; i < 12; ++i) {
      argv[5] = std::to_string(i);
      learn_helper(argc, argv);
    }
  }
  else if (string(argv[2]) == "learn_filter") {
    assert(argc == 9);
    Global::FilterableDistance = stoi(argv[6]);
    Global::FilterBiasX = stoi(argv[7]);
    Global::FilterBiasY = stoi(argv[8]);
    argc = 6;
    learn_helper(argc, argv);
  }
  else if (string(argv[2]) == "learn_fall") {
    assert(argc == 8);
    Global::FilterableDistance = stoi(argv[5]);
    Global::FilterBiasX = stoi(argv[6]);
    Global::FilterBiasY = stoi(argv[7]);
    argc = 6;
    remove("analysis_summary.csv");
    for (int i = 0; i < 12; ++i) {
      argv[5] = std::to_string(i);
      learn_helper(argc, argv);
    }
  }
  else if (string(argv[2]) == "learn_fdebug") {
    assert(argc == 8);
    Global::FilterableDistance = stoi(argv[5]);
    Global::FilterBiasX = stoi(argv[6]);
    Global::FilterBiasY = stoi(argv[7]);
    argc = 6;
    remove("analysis_summary.csv");
    for (int i = 6; i < 12; ++i) {
      argv[5] = std::to_string(i);
      learn_helper(argc, argv);
    }
  }
  else if (string(argv[2]) == "learn_debug") {
    argc += 1;
    argv.push_back("-");
    remove("analysis_summary.csv");
    for (int i = 6; i < 12; ++i) {
      argv[5] = std::to_string(i);
      learn_helper(argc, argv);
    }
  }
  else if (string(argv[2]) == "learn_good") {
    argc += 1;
    argv.push_back("-");
    argv[5] = "1";
    learn_helper(argc, argv);
    argv[5] = "3";
    learn_helper(argc, argv);
    argv[5] = "7";
    learn_helper(argc, argv);
    argv[5] = "9";
    learn_helper(argc, argv);
  }
  else if (string(argv[2]) == "llocs_all") {
    argc += 1;
    argv.push_back("-");
    for (int i = 20; i < 24; ++i) {
      argv[5] = std::to_string(i);
      learn_helper(argc, argv);
    }
  }
  else if (string(argv[2]) == "nav") {
    vector<unique_ptr<Point>> points;
    for (double i = 0; i < 10; ++i) {
      for (double j = 0; j < 10; ++j) {
        points.push_back(unique_ptr<Point>(new Point({i, j})));
      }
    }
    Map::TestInitMap(move(points));
    Navigation::TrySetDestinationFromCoords("9.0, 9.0");
    Location::TestSetCurrentNode(Map::NodeNearest(0, 0));
    Navigation::UpdateRoute();
    //for (auto iter = Navigation::route_begin(); iter != Navigation::route_end();
    //    ++iter) {
    //  printf("%3.0f %3.0f\n", (*iter)->point->x, (*iter)->point->y);
    }
  }
  else if (string(argv[2]) == "full") {
    string file_name = "Menu.png";
    std::ofstream stream(file_name.c_str(),std::ios::binary);
    if (!stream) {
      std::cout << "bitmap_image::save_image(): Error - Could not open file "
          << file_name << " for writing!" << std::endl;
      return;
    }
    stream.close();
    thread main_thread = thread(&Global::RunMainLoop);
    auto& display = Display::GetInstance();
    auto& keypad_handler = KeypadHandler::GetInstance();

    while(display.CurrentPage() != PAGE_MENU);
    this_thread::sleep_for(chrono::seconds(1));

    // Save main menu.
    display.SaveAsBitmap("Menu.bmp");

    keypad_handler.FakeStringEnter("3");

    auto result = Global::BlockForEvent(WINS_EVENT_SHUTDOWN_DONE, 5000);
    if (result == cv_status::timeout) {
      cout << "Main did not terminate. Forcing exit...\n";
      exit(1);
    } else {
      main_thread.join();
      cout << "Main terminted cleanly.\n";
    }
  } else if (string(argv[2]) == "imu1") {

  }
}

}
