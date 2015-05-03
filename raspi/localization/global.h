#ifndef GLOBAL_H
#define GLOBAL_H

#include <condition_variable>
#include <future>
#include <mutex>
#include <vector>
#include <string>
#include <thread>

namespace wins {

#define TEST 1

using namespace std;

enum WiFiVariant {
  WIFI_VARIANT_NONE      = 00,
  WIFI_VARIANT_CHI_SQ    = 01,
  WIFI_VARIANT_TOP1      = 02,
  WIFI_VARIANT_TOP_FEW   = 04,
  All                    = 010,
};

enum WinsEvent {
  WINS_EVENT_NONE               = 00,
  WINS_EVENT_POS_CHANGE         = 01,
  WINS_EVENT_ROUTE_CHANGE       = 02,
  WINS_EVENT_DEST_REACHED       = 04,
  WINS_EVENT_KEYPRESS           = 010,
  WINS_EVENT_BATTERY_LOW        = 020,
  WINS_EVENT_SHUTTING_DOWN      = 040,
  WINS_EVENT_SHUTDOWN_DONE      = 0100,
  WINS_EVENT_ALL                = 0777
};

struct BlockResult {
  cv_status status;
  WinsEvent events;
};

class Global {
 private:
  static WinsEvent event_flags_;
  static condition_variable display_event_pending_;
  static mutex event_mutex_;
  static thread::id mainthread_id_;
  static string shutdown_command_;
  static bool is_test_;

 public:
  static int FilterableDistance;
  static int FilterBiasX;
  static int FilterBiasY;
  static int LocationRFactor;
  static int LocationQFactor;
  static int IMU_R;
  static int IMU_QD;
  static int IMU_QV;
  static int IMU_QA;
  static double WiFiExp1;
  static double WiFiExp2;
  static WiFiVariant ScanVariant;
  static bool NoSleep;
  static int DurationOverride;
  static string MapFile;
  static vector<string> WiFiDevices;
  static int InitWiFiReadings;
  static int ReadingsPerUpdate;
  static double Scale;
  static atomic_bool DataDump;
  static string DumpFile;
  static mutex DumpMutex;

  static void RunMainLoop();
  static void Init();
  static void Destroy();
  static void SetEventFlag(WinsEvent flag);
  static bool IsFlagSet(WinsEvent flag);
  static BlockResult BlockForEvent(WinsEvent type, int millis = -1);
  static thread::id GetMainThreadId();
  static void ShutDown();
  static void SetTestMode();
  static bool IsTest();
};

}

#endif
