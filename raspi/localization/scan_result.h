#ifndef SCAN_RESULT_H
#define SCAN_RESULT_H

#include <utility>
#include <vector>

#include "common_utils.h"

struct Result {
  string name;
  int signal;
};

class ScanResult {
 private:
  vector<Result> scan_results_;
  ScanResult() { }

 public:
  static vector<ScanResult> Fetch();
  vector<Result>::const_iterator end () const;
  vector<Result>::const_iterator begin () const;
};

#endif //SCAN_RESULT_H
