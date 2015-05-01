#include <functional>
#include <future>
#include <stdexcept>
#include <type_traits>

#include "global.h"
#include "imu.h"
#include "kalman.h"
#include "location.h"
#include "log.h"
#include "map.h"
#include "wifi_estimate.h"

namespace wins {

using namespace std;

#define HIGH_VARIANCE 10000
#define NORMAL_MAX_DIST 20
#define MAX_MAX_DIST 500
#define MAX_UNKNOWN_COUNT 20

namespace {
bool close_enough(double a, double b) {
  return fabs(a - b) < 0.0001;
}
}

Eigen::MatrixXd Location::prev_X;
Eigen::MatrixXd Location::A;
Eigen::MatrixXd Location::A_t;
Eigen::MatrixXd Location::const_R;

vector<unique_ptr<WiFiEstimate>> Location::wifi_estimators_;
PointEstimate Location::point_estimate_;
atomic<kdtree::node<Point*>*> Location::current_node_(nullptr);
LocationVariant Location::variant_ = (LocationVariant)
    (LOCATION_VARIANT_FIXED_R | LOCATION_VARIANT_UPDATE_IMU);
int Location::unknown_count_ = 0;
double Location::max_distance_ = NORMAL_MAX_DIST;
std::chrono::time_point<std::chrono::steady_clock> Location::tp_epoch_;
std::chrono::time_point<std::chrono::steady_clock> Location::last_update_time_;

vector<int> default_channels =
    { 1, 6, 11, 48, 149, 36, 40, 157, 44, 153, 161 };

double Location::imu_x = -1;
double Location::imu_y = -1;
double Location::wifi_x = -1;
double Location::wifi_y = -1;
double Location::kalman_x = -1;
double Location::kalman_y = -1;
double Location::prev_x = -1;
double Location::prev_y = -1;

vector<PointEstimate> Location::GetWiFiReadings(int count) {
  vector<PointEstimate> estimates;
  vector<future<vector<PointEstimate>>> handles;
  for (auto& estimator : wifi_estimators_) {
    auto handle = async(launch::async, &WiFiEstimate::EstimateLocation,
          estimator.get(), WIFI_VARIANT_NONE, count);
    handles.push_back(move(handle));
  }
  for (auto& result : handles) {
    auto vec = result.get();
    if (vec.size() > 0) {
      estimates.push_back(vec[0]);
    }
  }
  return estimates;
}

void Location::InitialEstimate() {
  auto estimates = GetWiFiReadings(Global::InitWiFiReadings);
  DoKalmanUpdate(estimates);
}

void Location::InitKalman() {
  A = Eigen::MatrixXd::Identity(2,2);
  A_t = A.transpose();
  //const_R = 2 * Eigen::MatrixXd::Identity(2,2);
  const_R = 2 * Eigen::MatrixXd::Identity(2,2) * Global::LocationRFactor / 3;
  prev_X = Eigen::MatrixXd::Identity(2,1);
}

void Location::Init() {
  Imu::Init();
  InitKalman();
  last_update_time_ = tp_epoch_;

  wifi_estimators_.clear();
  for (auto device : Global::WiFiDevices) {
    wifi_estimators_.push_back(unique_ptr<WiFiEstimate>(
        new WiFiEstimate(unique_ptr<WifiScan>(
        new WifiScan(default_channels, device)))));
  }
  Map::UpdateLikelyPoints(numeric_limits<double>::max());
  InitialEstimate();
}

bool Location::DoKalmanUpdate(vector<PointEstimate> wifi_estimates) {
  int msecs;

  if (wifi_estimates.size() == 0) {
    FILE_LOG(logERROR) << "No WIFI Estimates.";
    unknown_count_ += 1;
    if (unknown_count_ == MAX_UNKNOWN_COUNT) {
      current_node_ = nullptr;
    }

    // Sleep for a while and retry.
    if (not Global::NoSleep) {
      this_thread::sleep_for(chrono::seconds(1));
    }
    return false;
  }

  unknown_count_ = 0;

  chrono::steady_clock::time_point new_update_time =
      chrono::steady_clock::now();
  if (last_update_time_ > tp_epoch_) {
    if (Global::DurationOverride > 0) {
      msecs = Global::DurationOverride;
    } else {
      msecs = chrono::duration_cast<std::chrono::milliseconds>(
        new_update_time - last_update_time_).count();
    }
  } else {
    msecs = 0;
  }

  Eigen::MatrixXd X(2,1);
  Eigen::MatrixXd P(2,2);
  Eigen::MatrixXd Z(wifi_estimates.size() * 2, 1);
  Eigen::MatrixXd H(wifi_estimates.size() * 2, 2);
  Eigen::MatrixXd H_t(2, wifi_estimates.size() * 2);
  Eigen::MatrixXd R(wifi_estimates.size() * 2, wifi_estimates.size() * 2);
  Eigen::MatrixXd Q(2,2);

  X = Imu::X.block<2,1>(0,0);
  P = Imu::P.block<2,2>(0,0);

  Q = Global::Scale * Eigen::MatrixXd::Identity(2,2) * (msecs / 1000) *
      Global::LocationQFactor / 4;
  R.setZero();

  for (size_t i = 0; i < wifi_estimates.size(); ++i) {
    Z(i*2, 0) = wifi_estimates[i].x_mean;
    Z(i*2+1, 0) = wifi_estimates[i].y_mean;
    H.block<2,2>(i*2, 0) << 1, 0,
                            0, 1;
    if (variant_ & LOCATION_VARIANT_FIXED_R) {
      R.block<2,2>(i*2, i*2) = const_R;
    } else {
      R(i*2, i*2) = wifi_estimates[i].x_var;
      R(i*2+1, i*2+1) = wifi_estimates[i].y_var;
    }
  }
  H_t = H.transpose();

  FILE_LOG(logLOCATION) << "I x = " << X(0,0) <<", y = " << X(1,0) << "\n";
  imu_x = X(0,0);
  imu_y = X(1,0);
  wifi_x = wifi_estimates[0].x_mean;
  wifi_y = wifi_estimates[0].y_mean;

  Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
  FILE_LOG(logIMU) << "R :\n" << R.format(CleanFmt) << "\n";
  FILE_LOG(logIMU) << "Q :\n" << Q.format(CleanFmt) << "\n";
  KalmanUpdate(X, P, Z, A, A_t, H, H_t, R, Q);

  kalman_x = X(0,0);
  kalman_y = X(1,0);
  prev_x = prev_X(0,0);
  prev_y = prev_X(1,0);

  if (last_update_time_ > tp_epoch_) {
    Eigen::Vector2d V = (X - prev_X) / msecs;
    Imu::X.block<2,1>(2,0) = V;
  }
  Imu::X.block<2,1>(0,0) = X;
  Imu::P.block<2,2>(0,0) = P;
  //FILE_LOG(logLOCATION) << P.format(CleanFmt) << "\n\n";

  last_update_time_ = new_update_time;
  prev_X = X;

  FILE_LOG(logLOCATION) << "L x = " << X(0,0) <<", y = " << X(1,0) << "\n";
  current_node_ = Map::NodeNearest(X(0,0), X(1,0));
	return true;
}

kdtree::node<Point*>* Location::GetCurrentNode() {
  return current_node_.load();
}

void Location::UpdateEstimate() {
  auto wifi_estimates = GetWiFiReadings(Global::ReadingsPerUpdate);
  double secs;
  if (Global::DurationOverride > 0) {
    secs = Global::DurationOverride / 1000;
  } else {
    secs = chrono::duration_cast<chrono::seconds>(
      chrono::steady_clock::now() - last_update_time_).count();
  }
  Imu::EstimateLocation(secs);
  Map::UpdateLikelyPoints(max_distance_ * Global::Scale);
  if (not DoKalmanUpdate(wifi_estimates) and max_distance_ <= MAX_MAX_DIST) {
    max_distance_ *= 1.5;
  } else if (not close_enough(max_distance_, NORMAL_MAX_DIST)) {
    max_distance_ /= 1.5;
  }
	//FILE_LOG(logLOCATION) <<"max dist = " << max_distance_ << "\n";
}

vector<FakeWifiScan*> Location::TestInit(vector<vector<Result>> setup_points,
    int num_wifis) {
  Imu::Init();
  InitKalman();
  last_update_time_ = tp_epoch_;
  current_node_ = nullptr;

  vector<FakeWifiScan*> fakescanners;

  wifi_estimators_.clear();
  for (int i = 0; i < num_wifis; ++i) {
    unique_ptr<FakeWifiScan> fakescanner(new FakeWifiScan(setup_points));
    fakescanners.push_back(fakescanner.get());
    wifi_estimators_.push_back(unique_ptr<WiFiEstimate>(
        new WiFiEstimate(unique_ptr<WifiScan>(move(fakescanner)))));
  }

  Map::UpdateLikelyPoints(numeric_limits<double>::max());
  InitialEstimate();

  return fakescanners;
}

vector<Result> Location::GetScans() {
  return wifi_estimators_[0]->GetScans();
}

void Location::TestSetCurrentNode(kdtree::node<Point*>* node) {
  current_node_ = node;
}

}
