#include "fake_wifiscan.h"

namespace wins {

FakeWifiScan::FakeWifiScan(vector<vector<Result>> results) : WifiScan({}) {
  for (auto r : results) {
    result_queue.push(unique_ptr<vector<Result>>(new vector<Result>(r)));
  }
}

vector<Result> FakeWifiScan::Fetch() {
  vector<Result> results = *result_queue.front();
  result_queue.pop();
  return results;
}

}
