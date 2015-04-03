#ifndef KALMAN_H
#define KALMAN_H

#include<Eigen/Dense>

namespace wins {

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
    const MatrixXd& Q);

}

#endif // KALMAN_H
