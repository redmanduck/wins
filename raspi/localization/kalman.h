#ifndef KALMAN_H
#define KALMAN_H

#include<Eigen/Dense>

using namespace Eigen;

template<typename T,
         int S,
         int O>
void KalmanUpdate(
    Matrix<T, S, 1>& X,
    Matrix<T, S, S>& P,
    const Matrix<T, O, 1>& Z,
    const Matrix<T, S, S>& A,
    const Matrix<T, S, S>& A_t,
    const Matrix<T, O, S>& H,
    const Matrix<T, S, O>& H_t,
    const Matrix<T, O, O>& R,
    const Matrix<T, S, S>& Q) {
  // State prediction.
  auto X_pred = A * X;

  // Covariance prediction.
  auto P_pred = A * P * A_t + Q;

  // Innovation.
  auto Y = Z - H * X_pred;

  // Innovaion covariance.
  auto I = H * P_pred * H_t + R;

  // Kalman gain.
  auto K = P_pred * H_t * I.inverse();

  // State update.
  X += K * Y;

  // Covariance update.
  P = (Matrix<T, S, S>::Identity() - K * H) * P_pred;
}

#endif // KALMAN_H
