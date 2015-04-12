#include <fstream>
#include <iostream>
#include <sstream>

#include "test_helpers.h"

#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/vector.hpp"
#include "map.h"
#include "point.h"
#include "wifi_estimate.h"

namespace wins {

#define SUM_LINES 20
#define MAX_UNKNOWN 3
#define BAD_READING 10000000

namespace {
  double mean(vector<double> v) {
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / v.size();
    return mean;
  }

  double std(vector<double> v) {
    std::vector<double> diff(v.size());
    std::transform(v.begin(), v.end(), diff.begin(),
                   std::bind2nd(std::minus<double>(), mean(v)));
    double sq_sum = std::inner_product(diff.begin(), diff.end(),
        diff.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / v.size());
    return stdev;
  }

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
        char buffer[100];
        sprintf(buffer, "%5.0f %5.0f %5.0f %5.0f\n", point->x, point->y,
            estimates[0].x_mean, estimates[0].y_mean);
        //cout << buffer;
        distances.push_back(sqrt(
            pow(point->x - estimates[0].x_mean, 2) +
            pow(point->y - estimates[0].y_mean, 2)));
        x_vars.push_back(estimates[0].x_var);
        y_vars.push_back(estimates[0].y_var);
      }
    }
    dp.mean = mean(distances);
    dp.std = std(distances);
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
    dp.std = std(distances);
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
    dp.std = std(distances);
    dp.mean_var_x = mean(x_vars);
    dp.mean_var_y = mean(y_vars);
  }
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

  using namespace std::placeholders;
  switch(stoi(argv[5])) {
    case 0: return;
    case 1: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, false,
        (WiFiVariant)(WIFI_VARIANT_NONE)); break;
    case 2: return;
    case 3: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, false,
        (WiFiVariant)(WIFI_VARIANT_CHI_SQ)); break;
    case 4: analysis_func = bind(
        MostProbableClubbedAnalysis, _1, _2, false); break;
    case 5: analysis_func = bind(
        MostProbableNotClubbedAnalysis, _1, _2, false); break;
    case 6: return;
    case 7: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, true,
        (WiFiVariant)(WIFI_VARIANT_NONE)); break;
    case 8: return;
    case 9: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, true,
        (WiFiVariant)(WIFI_VARIANT_CHI_SQ)); break;
    case 10: analysis_func = bind(
        MostProbableClubbedAnalysis, _1, _2, true); break;
    case 11: analysis_func = bind(
        MostProbableNotClubbedAnalysis, _1, _2, true); break;
    default: cout << "Unknown learn type\n"; exit(1);
  }

  vector<result> results;

  cout << ":::TYPE " << argv[5] << ":::\n";
  //printf("%7s %7s %7s %7s %3s %3s\n", "mean", "std", "mvx", "mvy", "e1", "e2");
  double m;
  double s;
  double mvx;
  double mvy;
  for (double exp1 = -5; exp1 <= 5; exp1 += 1) {
    for (double exp2 = -5; exp2 <= 5; exp2 += 1) {
      analysis_func(test_points, {exp1, exp2, m, s, mvx, mvy});
      if (not std::isnan(m)) {
        results.push_back(make_tuple(m, s, mvx, mvy, exp1, exp2));
        //printf("%7.2f %7.2f %7.2f %7.2f %3.1f %3.1f\n",
        //    m, s, mvx, mvy, exp1, exp2);
      }
    }
  }
  sort(results.begin(), results.end(),
      [](const result &a, const result &b) -> bool {
          return get<0>(a) < get<0>(b);
      });
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
