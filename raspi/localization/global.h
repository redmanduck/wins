#ifndef GLOBAL_H
#define GLOBAL_H

#include <condition_variable>
#include <mutex>
#include <vector>
#include <string>

namespace wins {

using namespace std;

enum WinsEvent {
  WINS_EVENT_NONE               = 00,
  WINS_EVENT_NAV_CHANGE         = 01,
  WINS_EVENT_DEST_REACHED       = 02,
  WINS_EVENT_KEYPRESS           = 04,
  WINS_EVENT_BATTERY_LOW        = 010,
  WINS_EVENT_BATTERY_CRITICAL   = 020,
  WINS_EVENT_ALL                = 077
};

class Global {
 private:
  static WinsEvent event_flags_;
  static condition_variable display_event_pending_;
  static mutex event_mutex_;

 public:
  static int FilterableDistance;
  static int FilterBiasX;
  static int FilterBiasY;
  static string MapFile;
  static vector<string> WiFiDevices;
  static int InitWiFiReadings;

  static void Init(int argc, char* argv[]);
  static void Destroy();
  static void SetEventFlag(WinsEvent flag);
  static bool IsFlagSet(WinsEvent flag);
  static void BlockForEvent(WinsEvent type);
  static void ShutDown();
};

}

#endif
