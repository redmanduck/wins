#ifndef FAKE_WIFISCAN_H
#define FAKE_WIFISCAN_H

#include <queue>

#include "scan_result.h"
#include "wifiscan.h"

namespace wins {

class FakeWifiScan : public WifiScan {
 public:
  queue<unique_ptr<vector<Result>>> result_queue;

  FakeWifiScan() : WifiScan({}) { }
  FakeWifiScan(vector<vector<Result>> results);
  virtual vector<Result> Fetch();
};

}

#endif
