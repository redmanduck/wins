#ifndef IMU_H
#define IMU_H

#include<Eigen/Dense>

#include "common_utils.h"

using namespace Eigen;

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
  static Matrix<double, OBSERVATIONS, OBSERVATIONS> R;  // Estimated sensor
                                                        // error covariance.
  static Matrix<double, SVARS, SVARS> Q;  // Estimated process error covariance.

  static Matrix<double, OBSERVATIONS, SVARS> H;
  static Matrix<double, SVARS, OBSERVATIONS> H_t;

  static vector<double> current_state;
  static vector<double> current_variance;
  static vector<double> sensor_variance;
  static vector<double> process_variance;

 public:
  static Matrix<double, SVARS, 1> X;      // Current state estimate.
  static Matrix<double, SVARS, SVARS> P;  // Current covariance estimate.

  static ImuResult FetchAll();
  static void Init();
  static PointEstimate EstimateLocation(ImuVariant v);
  // static PointEstimate EstimateLocation1(PointEstimate current);
};

#endif // IMU_H
