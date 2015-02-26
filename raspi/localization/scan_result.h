#ifndef SCAN_RESULT_H
#define SCAN_RESULT_H

#include "common_utils.h"
#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"

struct Result {
  string name;
  int signal;

  template<class Archive>
  void serialize(Archive & archive) {
    archive(name, signal);
  }
};

class ScanResult {
 public:
  static vector<Result> Fetch(string wlan);
};

#endif //SCAN_RESULT_H
