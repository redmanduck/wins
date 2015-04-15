#include <chrono>
#include <functional>
#include <stdexcept>

#ifndef ISOLATED_TEST
#include "common_utils.h"
#endif
#include "display.h"

#ifndef ISOLATED_TEST
#include "global.h"
#include "keypad_handler.h"
#include "navigation.h"
#include "spi_manager.h"
#endif

namespace wins {

using namespace std;

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
    return 0;
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
  Global::BlockForEvent(WINS_EVENT_KEYPRESS);
  KeypadHandler& keyhandler = KeypadHandler::GetInstance();

  return keyhandler.GetChar();
}

char Display::GetCharAndEcho() {
  char c = GetChar();
  PutString(string(1, c));
  return c;
}

void Display::PutString(string s, bool clear) {
  lock_guard<mutex> lock(glcd_mutex);
  if (clear) {
    ClearLine(current_line_);
    cursor_ = 0;
  }
  glcd_.drawstring_P(cursor_, current_line_, s.c_str());
  cursor_ += s.length();
  Flush();
}

string Display::GetStringAndEcho() {
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
  static int refresh_count = 0;
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

Page Display::Menu() {
  this_thread::sleep_for(chrono::seconds(1));
  ClearScreen();

  PutString("1. NAVIGATE");
  IncrmLine();
  IncrmLine();
  PutString("2. WHERE AM I?");
  IncrmLine();
  IncrmLine();
  PutString("3. SHUT DOWN");
  IncrmLine();
  IncrmLine();
  current_page_ = PAGE_MENU;

  while (true) {
    char option = GetChar();
    switch (option) {
      case '1':
      case '2':
          return PAGE_NAVIGATING;
      case '3': return PAGE_SHUT_DOWN;
      default : PutString("Enter 1, 2 or 3", true);
    }
  }
}

Page Display::DestinationPrompt() {
  SetFont(FONT_SIZE_MEDIUM, ALIGNMENT_LEFT);

  while (true) {
    ClearScreen();

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
    Global::BlockForEvent(WINS_EVENT_ALL);
    if (Global::IsFlagSet(WINS_EVENT_KEYPRESS)) {
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
    if (Global::IsFlagSet(WINS_EVENT_NAV_CHANGE)) {
      lock_guard<mutex> lock(Navigation::route_mutex);
      SetCurrentLine(3);
      PutString("Navigating...");
    }
    if (Global::IsFlagSet(WINS_EVENT_DEST_REACHED)) {
      Navigation::ResetDestination();
      return PAGE_DONE;
    }
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
  return PAGE_MENU;
}

Page Display::ShowPage(Page p) {
  switch (p) {
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

Display& Display::GetInstance() {
  static Display display;
  return display;
}

}
