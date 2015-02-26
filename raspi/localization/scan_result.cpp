#include "scan_result.h"

map<string, WifiScan> ScanResult::devices;
vector<int> ScanResult::default_channels =
    { 1, 6, 11, 48, 149, 36, 40, 157, 44, 153, 161 };

vector<Result> ScanResult::Fetch(string wlan, vector<int> channels) {
  auto device_iter = devices.find(wlan);
  if (device_iter == devices.end()) {
    device_iter = devices.emplace(wlan, WifiScan(default_channels, wlan)).first;
  }
  return device_iter->second.Fetch();
}
