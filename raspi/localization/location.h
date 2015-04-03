#ifndef LOCATION_H
#define LOCATION_H

#include<Eigen/Dense>

#include "common_utils.h"
#include "kdtree/node.hpp"
#include "point.h"
#include "wifi_estimate.h"
#include "wifiscan.h"

namespace wins {

enum LocationVariant {
  LOCATION_VARIANT_NONE       = 0,
  LOCATION_VARIANT_FIXED_R    = 1,
  LOCATION_VARIANT_UPDATE_IMU = 2
};

class Location {
 private:
  static LocationVariant variant_;

  static std::chrono::time_point<std::chrono::steady_clock> last_update_time_;

  static Eigen::Vector2d prev_X;
  static Eigen::MatrixXd P;
  static Eigen::MatrixXd A;
  static Eigen::MatrixXd A_t;
  static Eigen::MatrixXd Q;
  static Eigen::Vector2d const_R;

  static vector<unique_ptr<WiFiEstimate>> wifi_estimators_;
  static kdtree::node<Point*>* current_node_;
  static PointEstimate point_estimate_;

 public:
  static vector<PointEstimate> GetWiFiReadings(int count = 1);
  static void InitialEstimate();
  static void InitKalman();
  static void Init();
  static void DoKalmanUpdate(bool imu_valid, vector<PointEstimate>
      wifi_estimates, PointEstimate imu_estimate = {});
  static kdtree::node<Point*>* GetCurrentNode();
  static void UpdateEstimate();
};

}

#endif
