#ifndef WIFI_ESTIMATE_H
#define WIFI_ESTIMATE_H

#include "common_utils.h"
#include "scan_result.h"

enum WiFiVariant {
  WIFI_VARIANT_NONE      = 00,
  WIFI_VARIANT_CHI_SQ    = 01,
  WIFI_VARIANT_TOP1      = 02,
  WIFI_VARIANT_TOP_FEW   = 04,
  All                    = 010,
};

class WifiEstimate {
 public:
  static vector<PointEstimate> ClosestByMahalanobis(
      const vector<Result> *s, WiFiVariant v = WIFI_VARIANT_NONE,
      double realx = -1, double realy = 0,
      double exp1 = 4.5, double exp2 = -2.5, bool debug = false);
  // PointEstimate ClosestByMahalanobisVarWeighted(vector<Result> s);
  // PointEstimate MahalanobisTriangulated(vector<Result> s);
  static vector<PointEstimate> MostProbableClubbed(vector<Result>& s,
      double realx = -1, double realy = 0,
      double exp1 = 2.0, double exp2 = 0.0, bool debug = false);
  static vector<PointEstimate> MostProbableNotClubbed(vector<Result>& s,
      double realx = -1, double realy = 0,
      double exp1 = 1, double exp2 = 1, bool debug = false);
};

#endif //WIFI_ESTIMATE_H
