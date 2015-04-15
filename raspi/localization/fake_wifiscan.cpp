#include "fake_wifiscan.h"

namespace wins {

vector<Result> FakeWifiScan::Fetch() {
  vector<Result> results = result_queue.front();
  result_queue.pop();
  return results;
}

}
