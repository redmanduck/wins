#include "imu.h"
#include "kalman.h"

namespace wins {

MatrixXd Imu::X(SVARS, 1);
MatrixXd Imu::P(SVARS, SVARS);
MatrixXd Imu::R(OBSERVATIONS, OBSERVATIONS);
MatrixXd Imu::Q(SVARS, SVARS);
MatrixXd Imu::H(OBSERVATIONS, SVARS);
MatrixXd Imu::H_t(SVARS, OBSERVATIONS);

vector<double> Imu::current_state;
vector<double> Imu::current_variance;
vector<double> Imu::sensor_variance;
vector<double> Imu::process_variance;

void Imu::Init() {
  // Observation matrix.
  H.setZero();
  for (int i = 0; i < IMUS; ++i) {
    H(i * 2, 4) = 1;
    H(i * 2 + 1, 5) = 1;
  }
  H_t = H.transpose();

  R.setZero();
}

ImuResult Imu::FetchAll() {
  return ImuResult();
}

PointEstimate Imu::EstimateLocation(ImuVariant v) {
  auto imu_result = FetchAll();
  auto delta_t = imu_result.duration / imu_result.readings.size();

  Matrix<double, SVARS, 1> x_sum;
  Matrix<double, SVARS, SVARS> p_sum;
  x_sum.setZero();
  p_sum.setZero();

  auto X_initial = X;

  // State transition matrix.
  Matrix<double, SVARS, SVARS> A;
  A << 1, 0, delta_t, 0, delta_t * delta_t / 2, 0,
       0, 1, 0, delta_t, 0, delta_t * delta_t / 2,
       0, 0, 1, 0, delta_t, 0,
       0, 0, 0, 1, 0, delta_t,
       0, 0, 0, 0, 1, 0,
       0, 0, 0, 0, 0, 1;
  Matrix<double, SVARS, SVARS> A_t = A.transpose();

  for (auto& reading : imu_result.readings) {
    Matrix<double, OBSERVATIONS, 1> Z;

    // Copy X and Y acc values to measurement vector Z.
    for (int i = 0; i < OBSERVATIONS; ++i) {
      Z(i, 0) = reading[i];
    }
    KalmanUpdate(X, P, Z, A, A_t, H, H_t, R, Q);
    x_sum += X;
    p_sum += P;
  }
  auto average_state = x_sum.array() *
      Array<double, SVARS, 1>::Constant(
      1.0 / imu_result.readings.size());
  auto average_error = p_sum.array() *
      Array<double, SVARS, SVARS>::Constant(
      1.0 / imu_result.readings.size());

  if (v == IMU_VARIANT_KALMAN_DISTANCE_AVG) {
    X.block<2,1>(0,0) = average_state.block<2,1>(0,0);
    P.block<2,2>(0,0) = average_error.block<2,2>(0,0);
  } else if (v == IMU_VARIANT_KALMAN_ALL_AVERAGE) {
    X = average_state;
    P = average_error;
  } else if (v == IMU_VARIANT_KALMAN_VELOCITY_AVG) {
    X.block<2,1>(0,0) = X_initial.block<2,1>(0,0).array() +
        average_state.block<2,1>(2,0).array() *
        Array<double, 2, 1>::Constant(imu_result.duration);
    X.block<2,1>(2,0) = average_state.block<2,1>(2,0);
    P.block<2,2>(0,0) = average_error.block<2,2>(0,0);
  }

  return { X(0,0), P(0,0), X(1,0), P(1,1) };
}
/*
PointEstimate Imu::EstimateLocation1(PointEstimate current) {
  auto imu_result = FetchAll();
  vector<double> sum(0, IMUS * 4);
  for (auto& reading : imu_result.readings) {
    // Predict how much error.
    auto variance_prediction = current_variance + process_variance;

    // Compare readings against prediction.
    auto innovation = reading - current_state;

    // Compare real error against prediction.
    auto innovation_variance = variance_prediction + sensor_variance;

    // Moderate the prediction.
    auto kalman_gain = variance_prediction / innovation_variance;

    current_state = current_state + kalman_gain * innovation;
    current_variance = (vector<double>(0, STATE_SIZE) - kalman_gain) *
                        variance_prediction;
  }
  auto average_state = sum / vector<double>(imu_result.readings.size(),
      STATE_SIZE);
} */

}
