#ifndef GLOBAL_H
#define GLOBAL_H

#include <condition_variable>
#include <future>
#include <mutex>
#include <vector>
#include <string>
#include <thread>

namespace wins {

using namespace std;

enum WinsEvent {
  WINS_EVENT_NONE               = 00,
  WINS_EVENT_NAV_CHANGE         = 01,
  WINS_EVENT_DEST_REACHED       = 02,
  WINS_EVENT_KEYPRESS           = 04,
  WINS_EVENT_BATTERY_LOW        = 010,
  WINS_EVENT_SHUTTING_DOWN      = 020,
  WINS_EVENT_SHUTDOWN_DONE      = 040,
  WINS_EVENT_ALL                = 077
};

class Global {
 private:
  static WinsEvent event_flags_;
  static condition_variable display_event_pending_;
  static mutex event_mutex_;
  static thread::id mainthread_id_;
  static string shutdown_command_;

 public:
  static int FilterableDistance;
  static int FilterBiasX;
  static int FilterBiasY;
  static string MapFile;
  static vector<string> WiFiDevices;
  static int InitWiFiReadings;

  static void RunMainLoop();
  static void Init();
  static void Destroy();
  static void SetEventFlag(WinsEvent flag);
  static bool IsFlagSet(WinsEvent flag);
  static cv_status BlockForEvent(WinsEvent type, int millis = -1);
  static thread::id GetMainThreadId();
  static void ShutDown();
  static void SetTestMode();
};

}

#endif
