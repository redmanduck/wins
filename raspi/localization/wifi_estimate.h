#ifndef WIFI_ESTIMATE_H
#define WIFI_ESTIMATE_H

#include "common_utils.h"
#include "scan_result.h"

class WifiEstimate {
 public:
  PointEstimate ClosestByMahalanobis(ScanResult s);
  // PointEstimate ClosestByMahalanobisVarWeighted(ScanResult s);
  // PointEstimate MahalanobisTriangulated(ScanResult s);
  static PointEstimate MostProbableClubbed(ScanResult s);
  static PointEstimate MostProbableNotClubbed(ScanResult s);
};

#endif //WIFI_ESTIMATE_H
