#include <chrono>
#include <functional>
#include <stdexcept>

#ifndef ISOLATED_TEST
#include "common_utils.h"
#endif
#include "display.h"

#ifndef ISOLATED_TEST
#include "global.h"
#include "imu.h"
#include "keypad_handler.h"
#include "location.h"
#include "navigation.h"
#include "spi_manager.h"
#endif

namespace wins {

#define FLUSH_TO_SCREEN 1

using namespace std;

namespace {

int MaxLength(FontSize size) {
  if(size == FONT_SIZE_MEDIUM){
    return (int)(128/6);
  }
  throw runtime_error("Not Supported");
}

int MaxLines(FontSize size) {
  if(size == FONT_SIZE_MEDIUM){
     return 8;
  }
  throw runtime_error("Not Supported");
}

int FontWidth(FontSize size) {
  if(size == FONT_SIZE_MEDIUM){
    return 6;
  }
  throw runtime_error("Not Supported");
}

int route_change_count = 0;

}

void Display::ClearLine(int line) {
    lock_guard<mutex> lock(glcd_mutex);
    glcd_.fillrect(0, line*8 , 128, 8, 0);
    Flush();
}

void Display::ClearScreen() {
    lock_guard<mutex> lock(glcd_mutex);
    glcd_.clear();
    SetCurrentLine(1);
    Flush();
}

char Display::GetChar() {
  Flush();
  KeypadHandler& keyhandler = KeypadHandler::GetInstance();

  return keyhandler.GetChar();
}

char Display::GetCharAndEcho() {
  char c = GetChar();
  PutString(string(1, c));
  return c;
}

void Display::PutString(string s, bool clear) {
  if (clear) {
    ClearLine(current_line_);
    cursor_ = 0;
  }
  lock_guard<mutex> lock(glcd_mutex);
  glcd_.drawstring_P(cursor_, current_line_, s.c_str());
  cursor_ += s.length() * FontWidth(FONT_SIZE_MEDIUM);
  Flush();
}

string Display::GetStringAndEcho() {
  int char_count = 0;
  string buffer;
  char last_character;
  while (char_count < MaxLength(FONT_SIZE_MEDIUM)) {
    last_character = GetCharAndEcho();
    if (last_character == RETURN_CHARACTER) {
      break;
    }
    buffer += last_character;
  }
  return buffer;
}

void Display::Flush() {
  static int refresh_count = 0;
#ifdef FLUSH_TO_SCREEN
  if (Global::IsTest()) {
    system("clear");
    cout << "Screen refresh count = " << ++refresh_count << "\n";
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unique_ptr<bitmap_image> image = glcd_.getImage();
    for (unsigned int y = 0; y < 32; ++y) {
      for (unsigned int x = 0; x < 128; ++x) {
        image->get_pixel(x, y*2, r, g, b);
        bool up = r > 0;
        image->get_pixel(x, y*2+1, r, g, b);
        bool down = r > 0;
        if (up and down)
          cout << "█";
        else if (up)
          cout << "▀";
        else if (down)
          cout << "▄";
        else
          cout << " ";
      }
      cout <<"\n";
    }
  }
#endif
  flushed_ = true;
}

void Display::SetFont(FontSize size, Alignment al, int expected_width) {
  font_size_ = size;
  alignment_ = al;
  if (alignment_ == ALIGNMENT_LEFT) {
    cursor_ = 0;
  } else if (alignment_ == ALIGNMENT_CENTER) {
    cursor_ = (MaxLength(font_size_) - expected_width) / 2;
  }
}

void Display::SetCurrentLine(int line) {
  if (line == 0){
    throw runtime_error("Reserved space for PIC!");
  }
  current_line_ = line;
  if (line > MaxLines(font_size_)) {
    current_line_ = MaxLines(font_size_) - 1;
  }
  cursor_ = 0;
}

void Display::IncrmLine(){
    SetCurrentLine(++current_line_);
}

Display::Display()
  : glcd_(0) {
  font_size_ = FONT_SIZE_MEDIUM;
  cursor_ = 0;
  current_line_ = 0;
  Flush();
}

Page Display::Splash() {
  Flush();
  this_thread::sleep_for(chrono::seconds(1));
  return PAGE_CALIBRATE_PROMPT;
}

Page Display::CalibratePrompt() {
  ClearScreen();

  SetCurrentLine(3);
  PutString("Calibrate Required");
  IncrmLine();
  PutString("Start now? (Press 1)");
  current_page_ = PAGE_CALIBRATE_PROMPT;
  while (true) {
    char option = GetChar();
    if (option == '1') {
      return PAGE_CALBRATING;
    } else {
      return PAGE_NOCALIBRATE_WARN;
    }
  }
}

Page Display::NoCalibrateWarn() {
  ClearScreen();

  SetCurrentLine(2);
  PutString("Not calibrating will");
  IncrmLine();
  PutString("reduce accuracy");
  IncrmLine();
  PutString("Start calibrating");
  IncrmLine();
  PutString("now? (1 to begin)");
  current_page_ = PAGE_NOCALIBRATE_WARN;
  while (true) {
    char option = GetChar();
    if (option == '1') {
      return PAGE_CALBRATING;
    } else {
      return PAGE_MENU;
    }
  }
}

Page Display::Calibrating() {
  ClearScreen();

  SetCurrentLine(3);
  PutString("Calibrating...");
  current_page_ = PAGE_CALBRATING;
  Imu::Calibrate();
  return PAGE_MENU;
}

Page Display::Menu() {
  ClearScreen();

  PutString("1. Navigate");
  IncrmLine();
  PutString("2. Where am I?");
  IncrmLine();
  PutString("3. Calibrate");
  IncrmLine();
  PutString("4. Shut Down");
  IncrmLine();
  IncrmLine();
  current_page_ = PAGE_MENU;

  while (true) {
    char option = GetChar();
    switch (option) {
      case '1': return PAGE_DESTINATION_PROMPT;
      case '2': return PAGE_NAVIGATING;
      case '3': return PAGE_CALBRATING;
      case '4': return PAGE_SHUT_DOWN;
      default : PutString("Enter 1, 2, 3 or 4", true);
    }
  }
}

Page Display::DestinationPrompt() {
  SetFont(FONT_SIZE_MEDIUM, ALIGNMENT_LEFT);
  ClearScreen();

  while (true) {

    SetCurrentLine(2);
    PutString("Enter destination:", true);
    current_page_ = PAGE_DESTINATION_PROMPT;

    SetCurrentLine(3);
    if (Navigation::TrySetDestinationFromCoords(GetStringAndEcho())) {
      SetCurrentLine(1);
      PutString("Invalid location", true);
      break;
    }
  }
  return PAGE_NAVIGATING;
}

Page Display::Navigating() {
  ClearScreen();
  current_page_ = PAGE_NAVIGATING;
  while(true) {
    auto result = Global::BlockForEvent(WINS_EVENT_ALL);
    auto events = result.events;
    if (events & WINS_EVENT_KEYPRESS) {
      SetFont(FONT_SIZE_MEDIUM, ALIGNMENT_LEFT);
      SetCurrentLine(2);
      PutString("What's up?", true);

      SetCurrentLine(3);
      PutString("1: Menu 2: Continue", true);
      char input = GetChar();
      if (input == '1') {
        Navigation::ResetDestination();
        return PAGE_MENU;
      } else {
        continue;
      }
    }
    if (events & WINS_EVENT_POS_CHANGE) {
      SetCurrentLine(3);
      auto node = Location::GetCurrentNode();
      if (node != nullptr) {
        auto point = node->point;
        PutString("At (" + to_string(point->x) + ", " +
            to_string(point->y) + ")");
        auto node = Navigation::current_begin();
        if (node != Navigation::route_end()) {
          auto point = (*next(node))->point;
          IncrmLine();
          PutString("Go to (" + to_string(point->x) + ", " +
              to_string(point->y) + ")");
        }
      } else {
        PutString("Location Unknown");
      }
    }
    if (events & WINS_EVENT_ROUTE_CHANGE) {
      lock_guard<mutex> lock(Navigation::route_mutex);
      SetCurrentLine(5);
      PutString("Route changed " + to_string(++route_change_count));
    }
    if (events & WINS_EVENT_DEST_REACHED) {
      Navigation::ResetDestination();
      return PAGE_DONE;
    }
    Flush();
  }
}

Page Display::Done() {
  ClearScreen();
  SetCurrentLine(4);
  PutString("Destination reached");
  current_page_ = PAGE_DONE;
  GetChar();
  return PAGE_MENU;
}

Page Display::ShutDown() {
  ClearScreen();
  SetCurrentLine(4);
  PutString("Shutting Down");
  current_page_ = PAGE_SHUT_DOWN;
  return PAGE_SHUT_DOWN;
}

Page Display::FirstPage() {
  return PAGE_SPLASH;
}

Page Display::ShowPage(Page p) {
  switch (p) {
    case PAGE_SPLASH:               return Splash();
    case PAGE_CALIBRATE_PROMPT:     return CalibratePrompt();
    case PAGE_NOCALIBRATE_WARN:     return NoCalibrateWarn();
    case PAGE_CALBRATING:           return Calibrating();
    case PAGE_MENU:                 return Menu();
    case PAGE_NAVIGATING:           return Navigating();
    case PAGE_DESTINATION_PROMPT:   return DestinationPrompt();
    case PAGE_SHUT_DOWN:            return ShutDown();
    case PAGE_DONE:                 return Done();
    default: throw runtime_error("Unknown Page");
  }
}

Page Display::CurrentPage() {
  return current_page_;
}

void Display::SaveAsBitmap(string saveas){
    glcd_.savebitmap(saveas);
}

unique_ptr<uint8_t> Display::GetBufferCopy() {
  lock_guard<mutex> lock(glcd_mutex);
  unique_ptr<uint8_t> buffer_copy(new uint8_t[1024]);
  memcpy(buffer_copy.get(), glcd_.st7565_buffer, 1024);
  return buffer_copy;
}

Display& Display::GetInstance() {
  static Display display;
  return display;
}

}
