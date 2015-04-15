#ifndef IMU_H
#define IMU_H

#include<Eigen/Dense>

#include "common_utils.h"

namespace wins {

#define IMUS 1
#define READINGS 100
#define STATE_SIZE (IMUS * 4)
#define SVARS (IMUS * 6)
#define OBSERVATIONS (IMUS * 2)

enum ImuVariant {
  IMU_VARIANT_KALMAN_VANILLA,
  IMU_VARIANT_KALMAN_DISTANCE_AVG,
  IMU_VARIANT_KALMAN_VELOCITY_AVG,
  IMU_VARIANT_KALMAN_ALL_AVERAGE
};


struct ImuResult {
  vector<vector<double>> readings;
  double duration;
};

class Imu{
 private:
  static Eigen::MatrixXd R;  // Estimated sensor
  static Eigen::MatrixXd Q;  // Estimated process error covariance.

  static Eigen::MatrixXd H;
  static Eigen::MatrixXd H_t;

  static vector<double> current_state;
  static vector<double> current_variance;
  static vector<double> sensor_variance;
  static vector<double> process_variance;

 public:
  static Eigen::MatrixXd X;      // Current state estimate.
  static Eigen::MatrixXd P;  // Current covariance estimate.

  static ImuResult FetchAll();
  static void Init();
  static PointEstimate DoKalman(ImuResult imu_result,
      ImuVariant v = IMU_VARIANT_KALMAN_VANILLA);
  static PointEstimate EstimateLocation(
      ImuVariant v = IMU_VARIANT_KALMAN_VANILLA);
  // static PointEstimate EstimateLocation1(PointEstimate current);
};

}

#endif // IMU_H
