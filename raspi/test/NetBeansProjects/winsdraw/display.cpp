#include <chrono>
#include <functional>
#include <stdexcept>

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <cassert>
//#include "common_utils.h"
#include "display.h"
//#include "navigation.h"
//#include "spi_manager.h"

using namespace std;

char RETURN_CHARACTER = '\0';

/*
* Max character per line
* for given font size
*/
int MaxLength(FontSize size) {
  throw runtime_error("Not Implemented");
}

/*
*  Same thing as above but for rows
*/
int MaxLines(FontSize size) {
  throw runtime_error("Not Implemented");
}

//void Display::BlockForUpdate(DisplayUpdate type) {
//  unique_lock<mutex> lock(update_mutex_);
//  while (not update_ & type) {
//    display_update_pending_.wait(lock);
//    if (update_ & DISPLAY_UPDATE_BATTERY_CRITICAL) {
//      Global::ShutDown();
//    }
//  }
//}

void Display::ClearLine(int line) {
  throw runtime_error("Not Implemented");
}

void Display::ClearScreen() {
  throw runtime_error("Not Implemented");
}

void Display::PutChar(char character) {
  throw runtime_error("Not Implemented");
}

//char Display::GetChar() {
//  Flush();
//  BlockForUpdate(DISPLAY_UPDATE_KEYPRESS);
//  assert(SPI::input_buffer.length() > 0);
//
//  // Acquire lock to read buffer till end of method.
//  lock_guard<mutex> lock(SPI::buffer_mutex);
//
//  // Read the buffer and mark as read.
//  char input_char = SPI::input_buffer[0];
//  SPI::input_buffer.erase(0, 1);
//
//  return input_char;
//}

void Display::PutString(string s, bool clear) {
  if (clear) {
    ClearLine(current_line_);
    cursor_ = 0;
  }
  for (char c : s) {
    PutChar(c);
    cursor_ += 1;
    if (cursor_ >= MaxLength(font_size_))
      break;
  }
}

string Display::GetString() {
  int char_count = 0;
  string buffer;
  char last_character;
  while (char_count < MaxLength(FONT_SIZE_MEDIUM) or
      last_character == RETURN_CHARACTER) {
    last_character = GetChar();
    buffer += last_character;
  }
  return buffer;
}

void Display::Flush() {
  throw runtime_error("Not Implemented");
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
  current_line_ = line;
  if (line > MaxLines(font_size_)) {
    current_line_ = MaxLines(font_size_) - 1;
  }
}

//typedef void(*func_ptr)( Display*, NavMode );
//void Display::Menu() {
//  SPI::ShowMenu();
//  char option = GetChar();
//  switch (option) {
//    case '1': display_thread_ =
//        thread(&Display::DestinationPrompt, this, NAV_MODE_ROUTE); break;
//    case '2': display_thread_ =
//        thread(&Display::DestinationPrompt, this, NAV_MODE_LOCATE); break;
//    case '3': Global::ShutDown(); break;
//  }
//}

void Display::WhereAmI() {
  throw runtime_error("Not Implemented");
}

//void Display::DestinationPrompt(NavMode mode) {
//  SetFont(FONT_SIZE_MEDIUM, ALIGNMENT_LEFT);
//
//  while (true) {
//    ClearScreen();
//
//    SetCurrentLine(2);
//    PutString("Enter destination:", true);
//
//    SetCurrentLine(3);
//    if (Navigation::TrySetDestinationFromCoords(GetString())) {
//      SetCurrentLine(1);
//      PutString("Invalid location", true);
//      break;
//    }
//  }
//  display_thread_ = thread(&Display::Navigating, this, mode);
//}
//
//void Display::Navigating(NavMode mode) {
//  while(true) {
//    if (update_ == DISPLAY_UPDATE_NONE) {
//      BlockForUpdate(DISPLAY_UPDATE_ALL);
//    }
//    if (update_ & DISPLAY_UPDATE_KEYPRESS) {
//      SetFont(FONT_SIZE_MEDIUM, ALIGNMENT_LEFT);
//      SetCurrentLine(2);
//      PutString("What's up?", true);
//
//      SetCurrentLine(3);
//      PutString("1: Menu 2: Continue", true);
//      char input = GetChar();
//      if (input == '1') {
//        display_thread_ = thread(&Display::Menu, this);
//        return;
//      } else {
//        continue;
//      }
//    }
//    if (update_ & DISPLAY_UPDATE_NAV_CHANGE) {
//      lock_guard<mutex> lock(Navigation::route_mutex);
//
//      // TODO: Read the Route and Point estimate buffers and mark as read.
//    }
//    if (update_ & DISPLAY_UPDATE_DEST_REACHED) {
//      display_thread_ = thread(&Display::Done, this);
//      return;
//    }
//  }
//}
//
void Display::Done() {
  GetChar();
  display_thread_ = thread(&Display::Menu, this);
}

Display::Display() {
  display_thread_ = thread(&Display::Menu, this);
  font_size_ = FONT_SIZE_MEDIUM;
  cursor_ = 0;
  current_line_ = 0;
  flushed_ = true;
}