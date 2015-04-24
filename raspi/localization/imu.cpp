#include <thread>

#include "imu.h"
#include "kalman.h"

namespace wins {

using namespace Eigen;

#define HIGH_VARIANCE 10000
#define ACC_PRECISION 0
#define GYRO_PRECISION 0

namespace {
  void ParseIMU(vector<uint8_t> pic_data, Vector3d& acc,
      Quaternion<double>& quat) {
    acc(0) = ACC_PRECISION * pic_data[0] * 256.0 + pic_data[1];
    acc(1) = ACC_PRECISION * pic_data[2] * 256.0 + pic_data[3];
    acc(2) = ACC_PRECISION * pic_data[4] * 256.0 + pic_data[5];
    quat.w() = GYRO_PRECISION * pic_data[6] * 256.0 + pic_data[7];
    quat.x() = GYRO_PRECISION * pic_data[8] * 256.0 + pic_data[9];
    quat.y() = GYRO_PRECISION * pic_data[10] * 256.0 + pic_data[11];
    quat.z() = GYRO_PRECISION * pic_data[12] * 256.0 + pic_data[13];
    quat.normalize();
  }
}

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

ImuResult Imu::imu_buffer_;
mutex Imu::imu_buffer_mutex_;
atomic_bool Imu::calibrated_(false);
Quaternion<double> Imu::north_quat_inverse_ = Quaternion<double>::Identity();

void Imu::Init() {
  // Observation matrix.
  H.setZero();
  for (int i = 0; i < IMUS; ++i) {
    H(i * 2, 4) = 1;
    H(i * 2 + 1, 5) = 1;
  }
  H_t = H.transpose();

  X.setZero();
  P = HIGH_VARIANCE * MatrixXd::Identity(SVARS, SVARS);

  R = HIGH_VARIANCE * MatrixXd::Identity(OBSERVATIONS, OBSERVATIONS);
}

void Imu::AddReading(double x, double y) {
//  Vector3d raw_acc;
//  Quaternion<double> quat;
//  ParseIMU(pic_data, raw_acc, quat);
//
//  // Rotate acceleration vector to the default orientation of the gyro first
//  // (YPR all zeroes), and then rotate it north orientation.
//  Matrix3d rot_matrix = (quat * north_quat_inverse_).toRotationMatrix();
//  Vector3d acc = rot_matrix * raw_acc;
//
  lock_guard<mutex> lock(imu_buffer_mutex_);
  imu_buffer_.readings.push_back({ x, y });
}

void Imu::Calibrate() {
  //calibrated_ = false;

  //// Collect imu readings for 1 sec.
  //this_thread::sleep_for(chrono::seconds(1));

  //// Average IMU readings.
  //Vector4d vals = Vector4d::Zero();
  //lock_guard<mutex> lock(imu_buffer_mutex_);
  //for (auto r : imu_buffer_.readings) {
  //  vals(0) += r[3];
  //  vals(1) += r[4];
  //  vals(2) += r[5];
  //  vals(3) += r[6];
  //}
  //vals /= imu_buffer_.readings.size();

  //// Set the average value as the quat representing north orentation.
  //north_quat_inverse_ = Quaternion<double>(vals(0), vals(1), vals(2), vals(3))
  //    .inverse();

  //// Clear IMU buffer and mark as calibrated.
  //imu_buffer_.readings.clear();
  calibrated_ = true;
}

PointEstimate Imu::DoKalman(const ImuResult& imu_result, ImuVariant v) {
  auto delta_t = imu_result.duration / imu_result.readings.size();

  Matrix<double, SVARS, 1> x_sum = X;
  Matrix<double, SVARS, SVARS> p_sum = P;

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
      1.0 / (imu_result.readings.size() + 1);
  auto average_error = p_sum.array() *
      1.0 / (imu_result.readings.size() + 1);

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

PointEstimate Imu::EstimateLocation(ImuVariant v) {
  // If not calibrated don't use IMU.
  if (not calibrated_) {
    return { X(0,0), P(0,0), X(1,0), P(1,1) };
  }

  ImuResult results;

  // Create deep copy of imu_buffer_.
  {
    lock_guard<mutex> lock(imu_buffer_mutex_);
    results = imu_buffer_;
    imu_buffer_.readings.clear();
  }
  return DoKalman(results, v);
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
