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

  static chrono::steady_clock::time_point tp_epoch_;
  static chrono::steady_clock::time_point last_update_time_;

  static Eigen::MatrixXd prev_X;
  static Eigen::MatrixXd A;
  static Eigen::MatrixXd A_t;
  static Eigen::MatrixXd const_R;

  static vector<unique_ptr<WiFiEstimate>> wifi_estimators_;
  static kdtree::node<Point*>* current_node_;
  static PointEstimate point_estimate_;

 public:
  static vector<PointEstimate> GetWiFiReadings(int count = 1);
  static void InitialEstimate();
  static void InitKalman();
  static void Init();
  static void DoKalmanUpdate(vector<PointEstimate> wifi_estimates);
  static kdtree::node<Point*>* GetCurrentNode();
  static void UpdateEstimate();
};

}

#endif
