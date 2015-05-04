#include <fstream>
#include <iostream>
#include <sstream>

#include "test_helpers.h"

#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/vector.hpp"
#include "global.h"
#include "imu.h"
#include "location.h"
#include "map.h"
#include "point.h"
#include "wifi_estimate.h"

namespace wins {

#define SUM_LINES 20
#define MAX_UNKNOWN 3
#define BAD_READING 10000000

namespace {
  void MahalanobisAnalysis(vector<unique_ptr<Point>>& test_points,
      DebugParams dp, bool debug, WiFiVariant v) {
    vector<double> distances;
    vector<double> x_vars;
    vector<double> y_vars;
    WiFiEstimate w;
    for (auto&& point : test_points) {
      int unknown_count = 0;
      for (auto&& scan : point->scans) {
        auto estimates = w.ClosestByMahalanobis(
            scan, v, point->x, point->y, dp.exp1, dp.exp2, debug);
        if (estimates.size() == 0) {
          unknown_count += 1;
          if (unknown_count > MAX_UNKNOWN) {
            distances.push_back(BAD_READING);
            x_vars.push_back(BAD_READING);
            y_vars.push_back(BAD_READING);
          }
          continue;
        }
        unknown_count = 0;
        double distance = sqrt(
            pow(point->x - estimates[0].x_mean, 2) +
            pow(point->y - estimates[0].y_mean, 2));
        char buffer[100];
        sprintf(buffer, "%5.3f, %5.3f, %5.3f\n", distance,
            estimates[0].x_var, estimates[0].y_var);
        cout << buffer;
        distances.push_back(distance);
        x_vars.push_back(estimates[0].x_var);
        y_vars.push_back(estimates[0].y_var);
      }
    }
    dp.mean = mean(distances);
    dp.std = stddev(distances);
    dp.mean_var_x = mean(x_vars);
    dp.mean_var_y = mean(y_vars);
  }

  void MostProbableClubbedAnalysis(vector<unique_ptr<Point>>& test_points,
      DebugParams dp, bool debug) {
    vector<double> distances;
    vector<double> x_vars;
    vector<double> y_vars;
    WiFiEstimate w;
    for (auto&& point : test_points) {
      int unknown_count = 0;
      for (auto&& scan : point->scans) {
        auto estimates = w.MostProbableClubbed(scan,
            point->x, point->y, dp.exp1, dp.exp2, debug);
        assert(estimates.size() <= 1);
        if (estimates.size() == 0) {
          unknown_count += 1;
          if (unknown_count > MAX_UNKNOWN) {
            distances.push_back(BAD_READING);
            x_vars.push_back(BAD_READING);
            y_vars.push_back(BAD_READING);
          }
          continue;
        }
        unknown_count = 0;
        distances.push_back(sqrt(
            pow(point->x - estimates[0].x_mean, 2) +
            pow(point->y - estimates[0].y_mean, 2)));
        x_vars.push_back(estimates[0].x_var);
        y_vars.push_back(estimates[0].y_var);
      }
    }
    dp.mean = mean(distances);
    dp.std = stddev(distances);
    dp.mean_var_x = mean(x_vars);
    dp.mean_var_y = mean(y_vars);
  }

  void MostProbableNotClubbedAnalysis(vector<unique_ptr<Point>>& test_points,
      DebugParams dp, bool debug) {
    vector<double> distances;
    vector<double> x_vars;
    vector<double> y_vars;
    WiFiEstimate w;
    for (auto&& point : test_points) {
      int unknown_count = 0;
      for (auto&& scan : point->scans) {
        auto estimates = w.MostProbableNotClubbed(scan,
            point->x, point->y, dp.exp1, dp.exp2, debug);
        assert(estimates.size() <= 1);
        if (estimates.size() == 0) {
          unknown_count += 1;
          if (unknown_count > MAX_UNKNOWN) {
            distances.push_back(BAD_READING);
            x_vars.push_back(BAD_READING);
            y_vars.push_back(BAD_READING);
          }
          continue;
        }
        unknown_count = 0;
        distances.push_back(sqrt(
            pow(point->x - estimates[0].x_mean, 2) +
            pow(point->y - estimates[0].y_mean, 2)));
        x_vars.push_back(estimates[0].x_var);
        y_vars.push_back(estimates[0].y_var);
      }
    }
    dp.mean = mean(distances);
    dp.std = stddev(distances);
    dp.mean_var_x = mean(x_vars);
    dp.mean_var_y = mean(y_vars);
  }

  void LocationAnalysis(vector<unique_ptr<Point>>& test_points,
      DebugParams dp, int readings_per_update, int num_wifis) {
    vector<double> distances;
    vector<double> x_vars;
    vector<double> y_vars;
    WiFiEstimate w;

    // Walk at 1m/s.
    Global::DurationOverride = 1000;

    Global::LocationQFactor = dp.exp1;
    Global::LocationRFactor = dp.exp2;
    Global::ReadingsPerUpdate = readings_per_update;

    assert((int)test_points[0]->scans.size() >= Global::InitWiFiReadings);
    auto setup_points = vector<vector<Result>>(
        &test_points[0]->scans[0],
        &test_points[0]->scans[Global::InitWiFiReadings]);

    for (size_t i = 0; i < (test_points[0]->scans[0].size() / 5) * 5;
        i += readings_per_update * num_wifis) {
      auto fakescanners = Location::TestInit(setup_points, num_wifis);

      for (auto&& point : test_points) {
        int index = i;
        for (auto fakescanner : fakescanners) {
          for (int r = 0; r < readings_per_update; ++r) {
            // Wrap around if this scan does not have enough elements.
            // This is rare.
            auto& scan = point->scans[index % point->scans.size()];

            fakescanner->result_queue.push(unique_ptr<vector<Result>>(
                new vector<Result>(scan)));
            index += 1;
          }
        }

        //cout << "Testing x = " << point->x << ", y = " << point->y;
        Location::UpdateEstimate();

        //char buffer[100];
        //sprintf(buffer, "%5.0f %5.0f %5.0f %5.0f\n", point->x, point->y,
        //    estimates[0].x_mean, estimates[0].y_mean);
        //cout << buffer;
        auto node = Location::GetCurrentNode();
        auto distance = sqrt(pow(point->x - node->point->x, 2) +
            pow(point->y - node->point->y, 2));
        distances.push_back(distance);
        x_vars.push_back(-1);
        y_vars.push_back(-1);
      }
    }
    dp.mean = mean(distances);
    dp.std = stddev(distances);
    dp.mean_var_x = mean(x_vars);
    dp.mean_var_y = mean(y_vars);
  }
}

int AddNextSet(ifstream& fs, FakeWifiScan* fakescanner) {
  int imu_readings = 0;
  unique_ptr<vector<Result>> scans = unique_ptr<vector<Result>>(
      new vector<Result>);
  string line;
  while (imu_readings < 1000) {
    if (getline(fs, line)) {
      vector<string> parts = split(line, ',');
      if (parts[0] == "IMU") {
        Imu::AddReading(stod(parts[1]), stod(parts[2]), stod(parts[3]),
            stod(parts[4]), stod(parts[5]), stod(parts[6]), stod(parts[7]),
            stod(parts[8]));
        imu_readings += 1;
      } else if (parts[0] == "North") {
        Imu::SetNorthQuat(stod(parts[1]), stod(parts[2]), stod(parts[3]),
            stod(parts[4]));
      } else if (parts[0] == "WIFI") {
        scans->push_back(Result({parts[1], stod(parts[2])}));
      } else if (parts[0] == "-----") {
        if (fakescanner)
          fakescanner->result_queue.push(move(scans));
        return imu_readings;
      }
    } else {
      if (fakescanner)
        fakescanner->result_queue.push(move(scans));
      return imu_readings;
    }
  }
  if (fakescanner)
    fakescanner->result_queue.push(move(scans));
  return imu_readings;
}

void learn_helper(int argc, vector<string> argv) {
  assert(argc == 6);
  Map::InitMap(argv[3]);

  vector<unique_ptr<Point>> test_points;
  ifstream is(argv[4], ios::binary);
  cereal::BinaryInputArchive archive(is);

  archive(test_points);
  is.close();

  function<void(vector<unique_ptr<Point>>&, DebugParams)> analysis_func;
  int offset = 0;
  auto override_range = make_tuple(false, 0.0, 0.0);

  using namespace std::placeholders;
  switch(stoi(argv[5])) {
    case 0: return;
    case 1: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, false,
        (WiFiVariant)(WIFI_VARIANT_NONE));
        offset = 5; break;
    case 2: return;
    case 3: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, false,
        (WiFiVariant)(WIFI_VARIANT_CHI_SQ));
        offset = 5; break;
    case 4: analysis_func = bind(
        MostProbableClubbedAnalysis, _1, _2, false); break;
    case 5: analysis_func = bind(
        MostProbableNotClubbedAnalysis, _1, _2, false); break;
    case 6: return;
    case 7: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, true,
        (WiFiVariant)(WIFI_VARIANT_NONE));
        offset = 5; break;
    case 8: return;
    case 9: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, true,
        (WiFiVariant)(WIFI_VARIANT_CHI_SQ));
        offset = 5; break;
    case 10: analysis_func = bind(
        MostProbableClubbedAnalysis, _1, _2, true); break;
    case 11: analysis_func = bind(
        MostProbableNotClubbedAnalysis, _1, _2, true); break;
    case 12: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, true,
        (WiFiVariant)(WIFI_VARIANT_CHI_SQ));
        override_range = make_tuple(true, 5, 3); break;
    case 20: analysis_func = bind(
        LocationAnalysis, _1, _2, 1, 1); break;
    case 21: analysis_func = bind(
        LocationAnalysis, _1, _2, 1, 2); break;
    case 22: analysis_func = bind(
        LocationAnalysis, _1, _2, 2, 1); break;
    case 23: analysis_func = bind(
        LocationAnalysis, _1, _2, 2, 2); break;
    case 24: analysis_func = bind(
        LocationAnalysis, _1, _2, 1, 1);
        override_range = make_tuple(true, 5, 1); break;
    case 25: analysis_func = bind(
        LocationAnalysis, _1, _2, 1, 2);
        override_range = make_tuple(true, 5, 1); break;
    case 26: analysis_func = bind(
        LocationAnalysis, _1, _2, 2, 1);
        override_range = make_tuple(true, 5, 1); break;
    case 27: analysis_func = bind(
        LocationAnalysis, _1, _2, 2, 2);
        override_range = make_tuple(true, 5, 1); break;
    default: cout << "Unknown learn type\n"; exit(1);
  }

  vector<result> results;

  cout << ":::TYPE " << argv[5] << ":::\n";
  printf("%7s %7s %7s %7s %3s %3s\n", "mean", "std", "mvx", "mvy", "e1", "e2");
  double m;
  double s;
  double mvx;
  double mvy;
  if (not get<0>(override_range)) {
    for (double exp1 = 1; exp1 <= 10; exp1 += 1) {
      for (double exp2 = -5 + offset; exp2 <= 5 + offset; exp2 += 1) {
        analysis_func(test_points, {exp1, exp2, m, s, mvx, mvy});
        if (not std::isnan(m)) {
          results.push_back(make_tuple(m, s, mvx, mvy, exp1, exp2));
          printf("%7.2f %7.2f %7.2f %7.2f %3.1f %3.1f\n",
              m, s, mvx, mvy, exp1, exp2);
        }
      }
    }
    sort(results.begin(), results.end(),
        [](const result &a, const result &b) -> bool {
            return (get<0>(a) + get<1>(a)) < (get<0>(b) + get<1>(b));
        });
  } else {
    analysis_func(test_points, {
        get<1>(override_range), get<2>(override_range), m, s, mvx, mvy });
    if (not std::isnan(m)) {
      results.push_back(make_tuple(m, s, mvx, mvy, get<1>(override_range),
          get<2>(override_range)));
      printf("%7.2f %7.2f %7.2f %7.2f\n",
          m, s, mvx, mvy);
    }
  }

  ofstream out_file;
  out_file.open("analysis" + string(argv[5]) + "_results.csv");

  ofstream summary_file;
  summary_file.open("analysis_summary.csv", ios::app);

  char buffer[100];
  sprintf(buffer, "%7s %7s %7s %7s %5s %5s\n",
      "mean", "std", "mvx", "mvy", "e1", "e2");
  out_file << buffer;

  summary_file << ":::TYPE " << argv[5] << ":::\n";

  for (size_t i = 0; i < results.size(); ++i) {
    sprintf(buffer, "%7.2f %7.2f %7.2f %7.2f %5.2f %5.2f\n",
        get<0>(results[i]),
        get<1>(results[i]),
        get<2>(results[i]),
        get<3>(results[i]),
        get<4>(results[i]),
        get<5>(results[i]));
    out_file << buffer;

    if (i < SUM_LINES) {
      summary_file << buffer;
    }
  }
  out_file.close();

  summary_file << "\n";
  summary_file.close();
}

}
