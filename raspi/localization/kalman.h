#ifndef KALMAN_H
#define KALMAN_H

#include<Eigen/Dense>

namespace wins {

void KalmanUpdate(
    Eigen::MatrixXd& X,
    Eigen::MatrixXd& P,
    const Eigen::MatrixXd& Z,
    const Eigen::MatrixXd& A,
    const Eigen::MatrixXd& A_t,
    const Eigen::MatrixXd& H,
    const Eigen::MatrixXd& H_t,
    const Eigen::MatrixXd& R,
    const Eigen::MatrixXd& Q);

}

#endif // KALMAN_H
