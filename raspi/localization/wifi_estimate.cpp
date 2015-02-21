#include <algorithm>
#include <cmath>
#include <unordered_map>

#include "gamma.hpp"
#include "map.h"
#include "wifi_estimate.h"

#define MIN_DF 5
#define TOP_FEW_GOOD_PROB_THRESH 0.30
#define TOP_FEW_MIN_PROB_THRESH 0.35

// anonymous namespace
namespace {
  list<tuple<float, float, Point* const>> ComputePointStats(list<Result> s) {
    auto& current_likely_points = Map::CurrentLikelyPoints();
    list<tuple<float, float, Point* const>> point_stats;

    // Determine the probability of being at each of the possible points.
    for (auto& node : current_likely_points) {
      auto point = node->point;
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
      point_stats.push_back(make_tuple(total_prob, total_precision, point));
    }
    return point_stats;
  }

} // anonymous namespace

list<PointEstimate> WifiEstimate::ClosestByMahalanobis(const list<Result> *s,
    Variant v) {
  using stat = pair<float, Point*>;

  auto& current_likely_points = Map::CurrentLikelyPoints();
  vector<stat> point_stats;
  vector<pair<float, float>> stat_details;

  // Determine the probability of being at each of the possible points from
  // Mahalanobis distance.
  for (auto& node : current_likely_points) {
    auto point = node->point;
    float sum = 0;
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
      if (v & VARIANT_CHI_SQ)
        // Square of M-distance is ch-sqaured.
        // Weight of a point is the probability that the collected signal data
        // was taken at that point.
        point_stats.push_back(make_pair(
              1 - pchisq(sum, df),
              point));
      else
        // Weight of a point is proportional to the number of APs common to
        // that location and inversely proportinal to the M-distance.
        point_stats.push_back(make_pair(df / sqrt(sum), point));
      stat_details.push_back(make_pair(df, sqrt(sum)));
    }
  }

  float pred_x = 0;
  float pred_y = 0;
  float total_weight = 0;
  list<PointEstimate> estimates;
  if (v & VARIANT_TOP1 or v & VARIANT_TOP_FEW) {
    sort(point_stats.begin(), point_stats.end(),
        [](const stat &a, const stat &b) -> bool {
            return a.first > b.first;
        });
    if (v & VARIANT_TOP1) {
      estimates.push_back({
          point_stats[0].second->x,
          1 - point_stats[0].first,
          point_stats[0].second->y,
          1 - point_stats[0].first
      });
    } else if (v & VARIANT_TOP_FEW) {
      for (auto p_stat : point_stats) {
        // if (p_stat.first < TOP_FEW_MIN_PROB)
        //   continue;
        if (p_stat.first > TOP_FEW_GOOD_PROB_THRESH) {
          estimates.push_back({
              p_stat.second->x, 1 - p_stat.first,
              p_stat.second->y, 1 - p_stat.first });
        }
      }
    }
  } else {
    for (auto&& p_stat : point_stats) {
      float weight = p_stat.first;
      total_weight += weight;
      pred_x += p_stat.second->x * weight;
      pred_y += p_stat.second->y * weight;
    }

    pred_x /= total_weight;
    pred_y /= total_weight;

    float var_x = 0;
    float var_y = 0;
    for (auto&& p_stat : point_stats) {
      float weight = p_stat.first;
      total_weight += weight;
      var_x += pow(pred_x - p_stat.second->x, 2) * weight;
      var_y += pow(pred_y - p_stat.second->y, 2) * weight;
    }

    var_x /= total_weight;
    var_y /= total_weight;

    estimates.push_back({ /* x_mean */ nearbyint(pred_x),
                          /* x_var */ 1 / var_x,
                          /* y_mean */ nearbyint(pred_y),
                          /* y_var */ 1 / var_y});
  }
  return estimates;
}

PointEstimate WifiEstimate::MostProbableClubbed(list<Result> s) {
  auto point_stats = ComputePointStats(s);

  // Club all points with same x together and all points with the same y
  // together.
  unordered_map<int, pair<float, float>> x_points;
  unordered_map<int, pair<float, float>> y_points;
  for (auto&& p_stat : point_stats) {
    auto x = std::get<2>(p_stat)->x;
    auto x_point = x_points.find(x);
    if (x_point == x_points.end()) {
      y_points[x] = make_pair(std::get<0>(p_stat),
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
      y_points[x].first  += std::get<0>(p_stat);
      y_points[x].second += std::get<1>(p_stat);
    }
  }

  // Find the x-coordinate that we most likely are at.
  float total_weight_x = 0;
  float pred_x = 0;
  for (auto& x_point : x_points) {
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

  return { /* x_mean */ nearbyint(pred_x),
           /* x_var */ 1 / total_weight_x,
           /* y_mean */ nearbyint(pred_y),
           /* y_var */ 1 / total_weight_y };
}

PointEstimate WifiEstimate::MostProbableNotClubbed(list<Result> s) {
  auto point_stats = ComputePointStats(s);

  // Find the point that we most likely are at.
  float pred_x = 0;
  float pred_y = 0;
  float total_weight = 0;
  for (auto& p_stat : point_stats) {
    int weight = std::get<0>(p_stat) * std::get<1>(p_stat);
    total_weight += weight;
    pred_x += std::get<2>(p_stat)->x * weight;
    pred_y += std::get<2>(p_stat)->y * weight;
  }

  pred_x /= total_weight;
  pred_y /= total_weight;

  return { /* x_mean */ nearbyint(pred_x),
           /* x_var */ 1 / total_weight,
           /* y_mean */ nearbyint(pred_y),
           /* y_var */ 1 / total_weight };
}
