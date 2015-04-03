#ifndef KALMAN_H
#define KALMAN_H

#include<Eigen/Dense>

using namespace Eigen;

void KalmanUpdate(
    MatrixXd& X,
    MatrixXd& P,
    const MatrixXd& Z,
    const MatrixXd& A,
    const MatrixXd& A_t,
    const MatrixXd& H,
    const MatrixXd& H_t,
    const MatrixXd& R,
    const MatrixXd& Q) {
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
  P = (MatrixXd::Identity(P.rows(), P.cols()) - K * H) * P_pred;
}

#endif // KALMAN_H
