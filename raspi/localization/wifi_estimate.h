#ifndef WIFI_ESTIMATE_H
#define WIFI_ESTIMATE_H

#include "common_utils.h"
#include "scan_result.h"

enum Variant {
  VARIANT_NONE      = 00,
  VARIANT_CHI_SQ    = 01,
  VARIANT_TOP1      = 02,
  VARIANT_TOP_FEW   = 04,
  All               = 010,
};

class WifiEstimate {
 public:
  static list<PointEstimate> ClosestByMahalanobis(
      const list<Result> *s, Variant v);
  // PointEstimate ClosestByMahalanobisVarWeighted(list<Result> s);
  // PointEstimate MahalanobisTriangulated(list<Result> s);
  static PointEstimate MostProbableClubbed(list<Result> s);
  static PointEstimate MostProbableNotClubbed(list<Result> s);
};

#endif //WIFI_ESTIMATE_H
