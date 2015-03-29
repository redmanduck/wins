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

#define SUM_LINES 20

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
      double exp1, double exp2, double& m, double& s, bool debug,
      WiFiVariant v) {
    vector<double> distances;
    for (auto&& point : test_points) {
      for (auto&& scan : point->scans) {
        auto estimates = WifiEstimate::ClosestByMahalanobis(
            &scan, v, point->x, point->y, exp1, exp2, debug);
        assert(estimates.size() == 1);
        distances.push_back(sqrt(
            pow(point->x - estimates[0].x_mean, 2) +
            pow(point->y - estimates[0].y_mean, 2)));
      }
    }
    m = mean(distances);
    s = std(distances);
  }

  void MostProbableClubbedAnalysis(vector<unique_ptr<Point>>& test_points,
      double exp1, double exp2, double& m, double& s, bool debug) {
    vector<double> distances;
    for (auto&& point : test_points) {
      for (auto&& scan : point->scans) {
        auto estimates = WifiEstimate::MostProbableClubbed(scan,
            point->x, point->y, exp1, exp2, debug);
        assert(estimates.size() <= 1);
        if (estimates.size() == 0) {
          continue;
        }
        distances.push_back(sqrt(
            pow(point->x - estimates[0].x_mean, 2) +
            pow(point->y - estimates[0].y_mean, 2)));
      }
    }
    m = mean(distances);
    s = std(distances);
  }

  void MostProbableNotClubbedAnalysis(vector<unique_ptr<Point>>& test_points,
      double exp1, double exp2, double& m, double& s, bool debug) {
    vector<double> distances;
    for (auto&& point : test_points) {
      for (auto&& scan : point->scans) {
        auto estimates = WifiEstimate::MostProbableNotClubbed(scan,
            point->x, point->y, exp1, exp2, debug);
        assert(estimates.size() <= 1);
        if (estimates.size() == 0) {
          continue;
        }
        distances.push_back(sqrt(
            pow(point->x - estimates[0].x_mean, 2) +
            pow(point->y - estimates[0].y_mean, 2)));
      }
    }
    m = mean(distances);
    s = std(distances);
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

  function<void(vector<unique_ptr<Point>>&, double, double, double&,
      double&)> analysis_func;

  using namespace std::placeholders;
  switch(stoi(argv[5])) {
    case 0: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, _3, _4, _5, false,
        (WiFiVariant)(WIFI_VARIANT_TOP1)); break;
    case 1: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, _3, _4, _5, false,
        (WiFiVariant)(WIFI_VARIANT_NONE)); break;
    case 2: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, _3, _4, _5, false,
        (WiFiVariant)(WIFI_VARIANT_TOP1|WIFI_VARIANT_CHI_SQ)); break;
    case 3: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, _3, _4, _5, false,
        (WiFiVariant)(WIFI_VARIANT_CHI_SQ)); break;
    case 4: analysis_func = bind(
        MostProbableClubbedAnalysis, _1, _2, _3, _4, _5, false); break;
    case 5: analysis_func = bind(
        MostProbableNotClubbedAnalysis, _1, _2, _3, _4, _5, false); break;
    case 6: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, _3, _4, _5, true,
        (WiFiVariant)(WIFI_VARIANT_TOP1)); break;
    case 7: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, _3, _4, _5, true,
        (WiFiVariant)(WIFI_VARIANT_NONE)); break;
    case 8: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, _3, _4, _5, true,
        (WiFiVariant)(WIFI_VARIANT_TOP1|WIFI_VARIANT_CHI_SQ)); break;
    case 9: analysis_func = bind(
        MahalanobisAnalysis, _1, _2, _3, _4, _5, true,
        (WiFiVariant)(WIFI_VARIANT_CHI_SQ)); break;
    case 10: analysis_func = bind(
        MostProbableClubbedAnalysis, _1, _2, _3, _4, _5, true); break;
    case 11: analysis_func = bind(
        MostProbableNotClubbedAnalysis, _1, _2, _3, _4, _5, true); break;
    default: cout << "Unknown learn type\n"; exit(1);
  }

  vector<result> results;

  printf("%7s %7s %3s %3s\n", "mean", "std", "e1", "e2");
  double m;
  double s;
  for (double exp1 = -5; exp1 <= 5; exp1 += 0.5) {
    for (double exp2 = -5; exp2 <= 5; exp2 += 0.5) {
      analysis_func(test_points, exp1, exp2, m, s);
      if (not std::isnan(m)) {
        results.push_back(make_tuple(m, s, exp1, exp2));
        printf("%7.2f %7.2f %3.1f %3.1f\n", m, s, exp1, exp2);
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

  char buffer[50];
  sprintf(buffer, "%7s %7s %5s %5s\n", "mean", "std", "e1", "e2");
  out_file << buffer;

  summary_file << ":::TYPE " << argv[5] << ":::\n";

  for (size_t i = 0; i < results.size(); ++i) {
    sprintf(buffer, "%7.2f %7.2f %5.2f %5.2f\n",
        get<0>(results[i]),
        get<1>(results[i]),
        get<2>(results[i]),
        get<3>(results[i]));
    out_file << buffer;

    if (i < SUM_LINES) {
      summary_file << buffer;
    }
  }
  out_file.close();

  summary_file << "\n";
  summary_file.close();
}
