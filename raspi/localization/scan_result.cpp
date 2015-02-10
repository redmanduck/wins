#include "scan_result.h"

vector<Result>::const_iterator ScanResult::end () const {
  return scan_results_.cend();
}

vector<Result>::const_iterator ScanResult::begin () const {
  return scan_results_.cbegin();
}
vector<ScanResult> ScanResult::Fetch() {
  ScanResult s;
  s.scan_results_.push_back({ "AP 1", 50 });
  s.scan_results_.push_back({ "AP 2", 60 });
  s.scan_results_.push_back({ "AP 3", 70 });
  return { s };
}
