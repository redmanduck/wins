#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <tuple>
#include <unordered_map>

#include "gamma.hpp"
#include "global.h"
#include "log.h"
#include "map.h"
#include "wifi_estimate.h"

namespace wins {

#define MIN_OCCURENCE_PERCENT 0.9
#define MIN_DF 5
#define TOP_FEW_GOOD_PROB_THRESH 0.01
#define TOP_FEW_MIN_PROB_THRESH 0.35
#define MIN_WEIGHT 1E-37

// anonymous namespace
namespace {
  double mean(vector<double> v) {
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / v.size();
    return mean;
  }

  double distance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
  }

  vector<tuple<double, double, Point* const>> ComputePointStats(
      vector<Result> s, double realx, double realy, bool debug) {
    auto& current_likely_points = Map::CurrentLikelyPoints();
    vector<tuple<double, double, Point* const>> point_stats;

    // Determine the probability of being at each of the possible points.
    for (auto& node : current_likely_points) {
      auto point = node->point;
      if (debug and distance(point->x, point->y,
          realx - Global::FilterBiasX, realy - Global::FilterBiasY) >
          Global::FilterableDistance) {
        continue;
      }
      double total_prob = 0;
      double total_precision = 0;
      for (auto& mac : s) {
        auto stats = Map::Stats(point, mac.name, mac.signal);
        if (stats.mean() < 0) {
          continue;
        }
        total_prob += stats.prob() * stats.precision();
        total_precision += stats.precision();
      }
      point_stats.push_back(make_tuple(total_prob, total_precision, point));
    }
    return point_stats;
  }

} // anonymous namespace

vector<PointEstimate> WiFiEstimate::ClosestByMahalanobis(vector<Result> s,
    WiFiVariant v, double realx, double realy, double exp1, double exp2,
    bool debug) {
  using stat = tuple<double, Point*, double, double>;

  auto& current_likely_points = Map::CurrentLikelyPoints();
  vector<stat> point_stats;

  // Determine the probability of being at each of the possible points from
  // Mahalanobis distance.
  FILE_LOG(logWIFI) << "likely points = " << current_likely_points.size() << "\n";
  for (auto& node : current_likely_points) {
    auto point = node->point;
    if (debug and distance(point->x, point->y, realx - Global::FilterBiasX,
        realy - Global::FilterBiasY) > Global::FilterableDistance) {
      continue;
    }
    double sum = 0;
    int df = 0;
    for (auto& mac : s) {
			string name = mac.name;
			transform(name.begin(), name.end(), name.begin(), ::tolower);
      auto stats = Map::Stats(point, name, mac.signal);
      if (stats.mean() < 0) {
				//cout << name << " never seen before\n";
        continue;
      }
      sum += pow(stats.dist_mean(), 2);
      df += 1;
    }
    if (df < MIN_DF) {
      FILE_LOG(logWIFI) << "--------------------------df low!!!\n";
      continue;
    }

    //point_stats.push_back(ChiSquaredProbability(sum, df));
    if (df > 0 and sum > 0) {
      if (v & WIFI_VARIANT_CHI_SQ)
        // Square of M-distance is ch-sqaured.
        // Weight of a point is the probability that the collected signal data
        // was taken at that point.
        point_stats.push_back(make_tuple(
              pow(pow(10, df), exp1) * pow(100.0 * (1.0 - pchisq(sum, df)), exp2),
              point, df, pchisq(sum, df)));
      else
        // Weight of a point is proportional to the number of APs common to
        // that location and inversely proportinal to the M-distance.
        point_stats.push_back(make_tuple(pow(df, exp1) / pow(sum, exp2),
              point, pow(df, exp1), pow(sum, exp2)));
    }
  }

  //DEBUG CODE
  //char buffer[100];
  //sprintf(buffer, "\n%7s %7s %7s %7s %7s\n",
  //    "weight", "x", "y", "df", "metric");
  //FILE_LOG(logWIFI) << buffer;
  //sort(point_stats.begin(), point_stats.end(),
  //    [](const stat &a, const stat &b) -> bool {
  //        return get<0>(a) > get<0>(b);
  //    });
  //for (auto&& p_stat : point_stats) {
  //  sprintf(buffer, "%7.2e %7.1f %7.1f %7.2e %7.2e\n",
  //      get<0>(p_stat),
  //      get<1>(p_stat)->x,
  //      get<1>(p_stat)->y,
  //      get<2>(p_stat),
  //      get<3>(p_stat));
  //  FILE_LOG(logWIFI) << buffer;
  //}

//  if (v & WIFI_VARIANT_TOP1 or v & WIFI_VARIANT_TOP_FEW) {
//    sort(point_stats.begin(), point_stats.end(),
//        [](const stat &a, const stat &b) -> bool {
//            return get<0>(a) > get<0>(b);
//        });
//    if (v & WIFI_VARIANT_TOP1) {
//      estimates.push_back({
//          get<1>(point_stats[0])->x,
//          1 - get<0>(point_stats[0]),
//          get<1>(point_stats[0])->y,
//          1 - get<0>(point_stats[0])
//      });
//    } else if (v & WIFI_VARIANT_TOP_FEW) {
//      for (auto p_stat : point_stats) {
//        // if (p_stat.first < TOP_FEW_MIN_PROB)
//        //   continue;
//        if (get<0>(p_stat) > TOP_FEW_GOOD_PROB_THRESH) {
//          estimates.push_back({
//              get<1>(p_stat)->x, 1 - get<0>(p_stat),
//              get<1>(p_stat)->y, 1 - get<0>(p_stat)});
//        }
//      }
//    }
  vector<PointEstimate> estimates;
  if (point_stats.size() == 0) {
		FILE_LOG(logWIFI) << "-------------------Algorithm found no estimaes!!!\n";
    return estimates;
  }

  if (v & WIFI_VARIANT_TOP1 or v & WIFI_VARIANT_TOP_FEW) {
    throw runtime_error("Not Supported");
  } else {
    double pred_x = 0;
    double pred_y = 0;
    double total_weight = 0;
    for (auto&& p_stat : point_stats) {
      double weight = get<0>(p_stat);
      if (weight < MIN_WEIGHT) {
        weight = MIN_WEIGHT;
      }
      total_weight += weight;
      pred_x += get<1>(p_stat)->x * weight;
      pred_y += get<1>(p_stat)->y * weight;
    }

    pred_x /= total_weight;
    pred_y /= total_weight;

    double var_x = 0;
    double var_y = 0;
    double total_weight_var = 0;
    for (auto&& p_stat : point_stats) {
      double weight = get<2>(p_stat);
      total_weight_var += weight;
      var_x += pow(pred_x - get<1>(p_stat)->x, 2) * weight;
      var_y += pow(pred_y - get<1>(p_stat)->y, 2) * weight;
    }

    var_x /= total_weight_var;
    var_y /= total_weight_var;

    estimates.push_back({ /* x_mean */ pred_x,
                          /* x_var */ var_x,
                          /* y_mean */ pred_y,
                          /* y_var */ var_y});
  }

  //printf("%3s %3s %10s %7s %2s\n", "--x", "y", "chi", "ssum", "df");
  //for (int i = 0; i < point_stats.size(); ++i) {
  //  printf("%3.0f %3.0f %10.7f %7.2f %2d\n",
  //      get<1>(point_stats[i])->x,
  //      get<1>(point_stats[i])->y,
  //      get<0>(point_stats[i]),
  //      get<3>(point_stats[i]),
  //      get<2>(point_stats[i]));
  //}

  return estimates;
}

vector<PointEstimate> WiFiEstimate::MostProbableClubbed(vector<Result> s,
    double realx, double realy, double exp1, double exp2, bool debug) {
  auto point_stats = ComputePointStats(s, realx, realy, debug);

  // Club all points with same x together and all points with the same y
  // together.
  unordered_map<int, pair<double, double>> x_points;
  unordered_map<int, pair<double, double>> y_points;
  for (auto&& p_stat : point_stats) {
    auto x = std::get<2>(p_stat)->x;
    auto x_point = x_points.find(x);
    if (x_point == x_points.end()) {
      x_points[x] = make_pair(std::get<0>(p_stat),
          std::get<1>(p_stat));
    } else {
      x_points[x].first  += std::get<0>(p_stat);
      x_points[x].second += std::get<1>(p_stat);
    }

    auto y = std::get<2>(p_stat)->y;
    auto y_point = y_points.find(y);
    if (y_point == y_points.end()) {
      y_points[y] = make_pair(std::get<0>(p_stat),
          std::get<1>(p_stat));
    } else {
      y_points[y].first  += std::get<0>(p_stat);
      y_points[y].second += std::get<1>(p_stat);
    }
  }

  // Find the x-coordinate that we most likely are at.
  double total_weight_x = 0;
  double pred_x = 0;
  for (auto& x_point : x_points) {
    double weight = pow((x_point.second).first, exp1) *
        pow((x_point.second).second, exp2);
    total_weight_x += weight;
    pred_x += x_point.first * weight;
  }

  // Find the y-coordinate that we most likely are at.
  double total_weight_y = 0;
  double pred_y = 0;
  for (auto&& y_point : y_points) {
    double weight = pow((y_point.second).first, exp1) *
        pow((y_point.second).second, exp2);
    total_weight_y += weight;
    pred_y += y_point.first * weight;
  }

  pred_x /= total_weight_x;
  pred_y /= total_weight_y;

  double var_x = 0;
  double var_y = 0;
  double total_weight_x_sq = 0;
  double total_weight_y_sq = 0;
  for (auto&& x_point : x_points) {
    double weight = pow((x_point.second).first, exp1) *
        pow((x_point.second).second, exp2) / total_weight_x;
    total_weight_x_sq += weight * weight;
    var_x += pow(pred_x - x_point.first, 2) * weight;
  }
  for (auto&& y_point : y_points) {
    double weight = pow((y_point.second).first, exp1) *
        pow((y_point.second).second, exp2) / total_weight_y;
    total_weight_y_sq += weight * weight;
    var_y += pow(pred_y - y_point.first, 2) * weight;
  }

  vector<PointEstimate> estimates;

  if (total_weight_x < 0.0001 or total_weight_y < 0.0001) {
    return estimates;
  }

  estimates.push_back({ /* x_mean */ pred_x,
                        /* x_var */ var_x,
                        /* y_mean */ pred_y,
                        /* y_var */ var_y });
  return estimates;
}

vector<PointEstimate> WiFiEstimate::MostProbableNotClubbed(vector<Result> s,
    double realx, double realy, double exp1, double exp2, bool debug) {
  auto point_stats = ComputePointStats(s, realx, realy, debug);

  // Find the point that we most likely are at.
  double pred_x = 0;
  double pred_y = 0;
  double total_weight = 0;
  for (auto& p_stat : point_stats) {
    double weight = pow(get<0>(p_stat), exp1) * pow(get<1>(p_stat), exp2);
    total_weight += weight;
    pred_x += std::get<2>(p_stat)->x * weight;
    pred_y += std::get<2>(p_stat)->y * weight;
  }

  pred_x /= total_weight;
  pred_y /= total_weight;

  double var_x = 0;
  double var_y = 0;
  double total_weight_sq = 0;
  for (auto&& p_stat : point_stats) {
    double weight = pow(get<0>(p_stat), exp1) * pow(get<1>(p_stat), exp2) /
        total_weight;
    total_weight_sq += weight * weight;
    var_x += pow(pred_x - get<2>(p_stat)->x, 2) * weight;
    var_y += pow(pred_y - get<2>(p_stat)->y, 2) * weight;
  }

  vector<PointEstimate> estimates;
  estimates.push_back({ /* x_mean */ pred_x,
                        /* x_var */ var_x,
                        /* y_mean */ pred_y,
                        /* y_var */ var_y });
  return estimates;
}

vector<Result> AverageScans(vector<vector<Result>> scans) {
  map<string, vector<double>> macs;
  for (auto scan : scans) {
    for (auto result: scan) {
      auto iter = macs.find(result.name);
      if (iter == macs.end()) {
        iter = macs.emplace(result.name, vector<double>()).first;
      }
      iter->second.push_back(result.signal);
    }
  }
  vector<Result> averaged;
  size_t min_count = MIN_OCCURENCE_PERCENT * scans.size();
  for (auto& kv : macs) {
    if (kv.second.size() < min_count)
      continue;
    Result r = { kv.first, mean(kv.second) };
    averaged.push_back(r);
  }
  return averaged;
}

WiFiEstimate::WiFiEstimate(unique_ptr<WifiScan> scanner) {
  scanner_ = move(scanner);
}

vector<Result> WiFiEstimate::GetScans() {
  return scanner_->Fetch();
}

vector<PointEstimate> WiFiEstimate::EstimateLocation(
    WiFiVariant v,
    int read_count) {
  if (not scanner_) {
    throw runtime_error("No scanner");
  }
  if (read_count > 1) {
    FILE_LOG(logWIFI) << "AVERAGING...\n";
    vector<vector<Result>> scans;
    for (int i = 0; i < read_count; ++i) {
      scans.push_back(scanner_->Fetch());
    }
    if (Global::WiFiExp1 != 0) {
      return ClosestByMahalanobis(AverageScans(scans), v, 0, 0, Global::WiFiExp1,
          Global::WiFiExp2, false);
    } else {
      return ClosestByMahalanobis(AverageScans(scans), v);
    }
  } else {
    FILE_LOG(logWIFI) << "Fetching fresh\n";
    vector<Result> results = scanner_->Fetch();
    if (Global::DataDump) {
      lock_guard<mutex> lock(Global::DumpMutex);
      ofstream dumpfile(Global::DumpFile, ofstream::app);
      for (auto r : results) {
        dumpfile << "WIFI," << r.name << "," << r.signal << "\n";
      }
      dumpfile << "-----";
      dumpfile.close();
    }
    FILE_LOG(logWIFI) << "size = " << results.size() << "\n";
    vector<PointEstimate> wifi_estimates;
    if (Global::WiFiExp1 != 0) {
      wifi_estimates = ClosestByMahalanobis(results, v, 0, 0, Global::WiFiExp1,
          Global::WiFiExp2, false);
    } else {
      wifi_estimates = ClosestByMahalanobis(results, v);
    }
    if (wifi_estimates.size() > 0) {
      FILE_LOG(logLOCATION) << "W x = " << wifi_estimates[0].x_mean <<", y = " <<
          wifi_estimates[0].y_mean << "\n";
    }
    return wifi_estimates;
  }
}

}
