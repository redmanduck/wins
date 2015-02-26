#ifndef WIFI_ESTIMATE_H
#define WIFI_ESTIMATE_H

#include "common_utils.h"
#include "scan_result.h"

enum WiFiVariant {
  WIFI_VARIANT_NONE      = 00,
  WIFI_VARIANT_CHI_SQ    = 01,
  WIFI_VARIANT_TOP1      = 02,
  WIFI_VARIANT_TOP_FEW   = 04,
  All               = 010,
};

class WifiEstimate {
 public:
  static vector<PointEstimate> ClosestByMahalanobis(
      const vector<Result> *s, WiFiVariant v);
  // PointEstimate ClosestByMahalanobisVarWeighted(vector<Result> s);
  // PointEstimate MahalanobisTriangulated(vector<Result> s);
  static PointEstimate MostProbableClubbed(vector<Result> s);
  static PointEstimate MostProbableNotClubbed(vector<Result> s);
};

#endif //WIFI_ESTIMATE_H
