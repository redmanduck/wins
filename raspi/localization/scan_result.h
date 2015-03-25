#ifndef SCAN_RESULT_H
#define SCAN_RESULT_H

#include <map>
#include <string>

#include "common_utils.h"
#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"
#include "wifiscan.h"

using namespace std;

class WifiScan;

struct Result {
  string name;
  int signal;

  template<class Archive>
  void serialize(Archive & archive) {
    archive(name, signal);
  }
};

class ScanResult {
 private:
  static map<string, WifiScan> devices;
  static vector<int> default_channels;

 public:
  static vector<Result> Fetch(string wlan,
      vector<int> channels = default_channels);
};

#endif //SCAN_RESULT_H
