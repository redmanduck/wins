#ifndef DISPLAY_H
#define DISPLAY_H

#include <condition_variable>

#include "common_utils.h"

enum DisplayUpdate {
  DISPLAY_UPDATE_NONE               = 00,
  DISPLAY_UPDATE_ROUTE_CHANGE       = 01,
  DISPLAY_UPDATE_KEYPRESS           = 02,
  DISPLAY_UPDATE_BATTERY_LOW        = 04,
  DISPLAY_UPDATE_BATTERY_CRITICAL   = 08,
};

enum FontSize {
  FONT_SIZE_MEDIUM,
  FONT_SIZE_LARGE,
  FONT_SIZE_XLARGE
};

enum Alignment {
  ALIGNMENT_LEFT,
  ALIGNMENT_CENTER
};

class Display {
 private:
  enum CurrentScreen {
    CURRENT_SCREEN_BOOT,
    CURRENT_SCREEN_DESTINATION,
    CURRENT_SCREEN_NAVIGATING,
    CURRENT_SCREEN_NOW,
    CURRENT_SCREEN_ARRIVED,
    CURRENT_SCREEN_QUIT
  };
  static CurrentScreen current_screen_;
  static thread display_thread_;
  static mutex dummy_;

 public:
  static condition_variable display_update_pending;
  static DisplayUpdate update;

  static void Start();
};
