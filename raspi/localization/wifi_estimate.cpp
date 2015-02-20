#include <algorithm>
#include <cmath>
#include <unordered_map>

#include "gamma.hpp"
#include "map.h"
#include "wifi_estimate.h"

#define MIN_DF 5

// anonymous namespace
namespace {
  vector<pair<float, float>> ComputePointStats(list<Result> s) {
    auto& current_likely_points = Map::CurrentLikelyPoints();
    vector<pair<float, float>> point_stats;
 
    // Determine the probability of being at each of the possible points.
    for (auto& point : current_likely_points) {
      float total_prob = 0;
      float total_precision = 0;
      for (auto& mac : s) {
        auto stats = Map::Stats(point, mac.name, mac.signal);
        if (stats.mean() < 0) {
          continue;
        }
        total_prob += stats.prob() * stats.precision();
        total_precision += stats.precision();
      }
      point_stats.push_back(make_pair(total_prob, total_precision));
    }
    return point_stats;
  }

} // anonymous namespace

PointEstimate WifiEstimate::ClosestByMahalanobis(const list<Result> *s,
    Variant v) {
  using stat = pair<float, const Point*>;

  auto&& current_likely_points = Map::CurrentLikelyPoints();
  vector<stat> point_stats;
  vector<pair<float, float>> stat_details;

  // Determine the probability of being at each of the possible points from
  // Mahalanobis distance.
  for (auto&& point : current_likely_points) {
    float sum = 0;
    int df = 0;
    for (auto&& mac : *s) {
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
      if (v & VARIANT_CHI_SQ)
        point_stats.push_back(make_pair(
              1 - pchisq(sum, df),
              &point));
      else
        point_stats.push_back(make_pair(df / sqrt(sum), &point));
      stat_details.push_back(make_pair(df, sqrt(sum)));
    }
  }

  float pred_x = 0;
  float pred_y = 0;
  float total_weight = 0;
  if (v & VARIANT_TOP1 or v & VARIANT_TOP_FEW) {
    sort(point_stats.begin(), point_stats.end(),
        [](const stat &a, const stat &b) -> bool {
            return a.first > b.first;
        });
    if ((v & VARIANT_TOP1) and not (v & VARIANT_TOP_FEW)) {
      pred_x = point_stats[0].second->x;
      pred_y = point_stats[0].second->y;
    } else if (not (v & VARIANT_TOP1) and (v & VARIANT_TOP_FEW)) {
      if (point_stats[0].first - point_stats[1].first < 0.1) {
        cout << "clash!!!\n";
      }
      pred_x = point_stats[0].second->x;
      pred_y = point_stats[0].second->y;
      //throw "NotImplemented";
    }
  } else {
    for (auto&& stat : point_stats) {
      float weight = stat.first;
      total_weight += weight;
      pred_x += stat.second->x * weight;
      pred_y += stat.second->y * weight;
    } 
    
    pred_x /= total_weight;
    pred_y /= total_weight;
  }
  
  float var_x = 0;
  float var_y = 0;
  for (auto&& stat : point_stats) {
    float weight = stat.first;
    total_weight += weight;
    var_x += pow(pred_x - stat.second->x, 2) * weight;
    var_y += pow(pred_y - stat.second->y, 2) * weight;
  }

  var_x /= total_weight;
  var_y /= total_weight;

  return { /* x_mean */ (int)nearbyint(pred_x), 
           /* x_var */ 1 / var_x, 
           /* y_mean */ (int)nearbyint(pred_y),
           /* y_var */ 1 / var_y};
}
  
PointEstimate WifiEstimate::MostProbableClubbed(list<Result> s) {
  auto&& current_likely_points = Map::CurrentLikelyPoints();
  auto point_stats = ComputePointStats(s);

  // Club all points with same x together and all points with the same y
  // together.
  unordered_map<int, pair<float, float>> x_points;
  unordered_map<int, pair<float, float>> y_points;
  for (size_t i = 0; i < current_likely_points.size(); ++i) {
    auto x = current_likely_points[i].x;
    auto x_point = x_points.find(x);
    if (x_point == x_points.end()) {
      x_points[x] = point_stats[i];
    } else {
      x_points[x].first += point_stats[i].first;
      x_points[x].second += point_stats[i].second;
    }

    auto y = current_likely_points[i].y;
    auto y_point = y_points.find(y);
    if (y_point == y_points.end()) {
      y_points[y] = point_stats[i];
    } else {
      y_points[y].first += point_stats[i].first;
      y_points[y].second += point_stats[i].second;
    }
  }

  // Find the x-coordinate that we most likely are at.
  float total_weight_x = 0;
  float pred_x = 0;
  for (auto&& x_point : x_points) {
    int weight = (x_point.second).first * (x_point.second).second;
    total_weight_x += weight;
    pred_x += x_point.first * weight;
  }

  // Find the y-coordinate that we most likely are at.
  float total_weight_y = 0;
  float pred_y = 0;
  for (auto&& y_point : y_points) {
    int weight = (y_point.second).first * (y_point.second).second;
    total_weight_y += weight;
    pred_y += y_point.first * weight;
  }

  pred_x /= total_weight_x;
  pred_y /= total_weight_y;

  return { /* x_mean */ (int)nearbyint(pred_x), 
           /* x_var */ 1 / total_weight_x, 
           /* y_mean */ (int)nearbyint(pred_y),
           /* y_var */ 1 / total_weight_y };
}

PointEstimate WifiEstimate::MostProbableNotClubbed(list<Result> s) {
  auto&& current_likely_points = Map::CurrentLikelyPoints();
  auto point_stats = ComputePointStats(s);

  // Find the point that we most likely are at.
  float pred_x = 0;
  float pred_y = 0;
  float total_weight = 0;
  for (size_t i = 0; i < current_likely_points.size(); ++i) {
    int weight = point_stats[i].first * point_stats[i].second;
    total_weight += weight;
    pred_x += current_likely_points[i].x * weight;
    pred_y += current_likely_points[i].y * weight;
  }

  pred_x /= total_weight;
  pred_y /= total_weight;

  return { /* x_mean */ (int)nearbyint(pred_x), 
           /* x_var */ 1 / total_weight, 
           /* y_mean */ (int)nearbyint(pred_y),
           /* y_var */ 1 / total_weight };
}
