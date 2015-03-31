#include <algorithm>
#include <cmath>
#include <cstdio>
#include <tuple>
#include <unordered_map>

#include "gamma.hpp"
#include "map.h"
#include "wifi_estimate.h"

#define MIN_DF 5
#define TOP_FEW_GOOD_PROB_THRESH 0.01
#define TOP_FEW_MIN_PROB_THRESH 0.35

// anonymous namespace
namespace {
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

vector<PointEstimate> WifiEstimate::ClosestByMahalanobis(const vector<Result> *s,
    WiFiVariant v, double realx, double realy, double exp1, double exp2,
    bool debug) {
  using stat = tuple<double, Point*, int, double>;

  auto& current_likely_points = Map::CurrentLikelyPoints();
  vector<stat> point_stats;

  // Determine the probability of being at each of the possible points from
  // Mahalanobis distance.
  for (auto& node : current_likely_points) {
    auto point = node->point;
    if (debug and distance(point->x, point->y, realx - Global::FilterBiasX,
        realy - Global::FilterBiasY) > Global::FilterableDistance) {
      continue;
    }
    double sum = 0;
    int df = 0;
    for (auto& mac : *s) {
      auto stats = Map::Stats(point, mac.name, mac.signal);
      if (stats.mean() < 0) {
        continue;
      }
      sum += pow(stats.dist_mean(), 2);
      df += 1;
    }
    if (df < MIN_DF) {
      //cout << "df low!!!";
      continue;
    }

    //point_stats.push_back(ChiSquaredProbability(sum, df));
    if (df > 0 and sum > 0) {
      if (v & WIFI_VARIANT_CHI_SQ)
        // Square of M-distance is ch-sqaured.
        // Weight of a point is the probability that the collected signal data
        // was taken at that point.
        point_stats.push_back(make_tuple(
              pow(df, exp1) * pow(100.0 * (1.0 - pchisq(sum, df)), exp2),
              point, df, sqrt(sum)));
      else
        // Weight of a point is proportional to the number of APs common to
        // that location and inversely proportinal to the M-distance.
        point_stats.push_back(make_tuple(pow(df, exp1) * pow(sqrt(sum), exp2),
              point, df, sqrt(sum)));
    }
  }

  vector<PointEstimate> estimates;
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
  if (v & WIFI_VARIANT_TOP1 or v & WIFI_VARIANT_TOP_FEW) {
    throw runtime_error("Not Supported");
  } else {
    double pred_x = 0;
    double pred_y = 0;
    double total_weight = 0;
    for (auto&& p_stat : point_stats) {
      double weight = get<0>(p_stat);
      total_weight += weight;
      pred_x += get<1>(p_stat)->x * weight;
      pred_y += get<1>(p_stat)->y * weight;
    }

    pred_x /= total_weight;
    pred_y /= total_weight;

    double var_x = 0;
    double var_y = 0;
    double total_weight_sq = 0;
    for (auto&& p_stat : point_stats) {
      double weight = get<0>(p_stat) / total_weight;
      total_weight_sq += weight * weight;
      var_x += pow(pred_x - get<1>(p_stat)->x, 2) * weight;
      var_y += pow(pred_y - get<1>(p_stat)->y, 2) * weight;
    }

    estimates.push_back({ /* x_mean */ nearbyint(pred_x),
                          /* x_var */ var_x,
                          /* y_mean */ nearbyint(pred_y),
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

vector<PointEstimate> WifiEstimate::MostProbableClubbed(vector<Result>& s,
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

  estimates.push_back({ /* x_mean */ nearbyint(pred_x),
                        /* x_var */ var_x,
                        /* y_mean */ nearbyint(pred_y),
                        /* y_var */ var_y });
  return estimates;
}

vector<PointEstimate> WifiEstimate::MostProbableNotClubbed(vector<Result>& s,
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
  estimates.push_back({ /* x_mean */ nearbyint(pred_x),
                        /* x_var */ var_x,
                        /* y_mean */ nearbyint(pred_y),
                        /* y_var */ var_y });
  return estimates;
}
