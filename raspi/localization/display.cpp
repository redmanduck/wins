#include <chrono>
#include <exception>
#include <thread>

#include "display.h"

CurrentScreen Display::current_screen_ = CURRENT_SCREEN_BOOT;
thread Display::display_thread_;

condition_variable Display::display_update_pending;
DisplayUpdate Display::update = DISPLAY_UPDATE_NONE;

void Display::Start() {
  display_thread_ = thread(DestinationPrompt);
}

void Display::ClearLine() {
  throw exception("Not Implemented");
}

void Display::PutChar(char character) {
  throw exception("Not Implemented");
}

char Display::GetChar() {
  while(true) {
    // Wait for user input and acquire lock.
    lock_guard<mutex>(SPI::buffer_mutex);
    while (update == DISPLAY_UPDATE_NONE) {
      display_update_pending.wait(SPI::buffer_mutex);
    }
    if (update & DISPLAY_UPDATE_KEYPRESS)
      break;
    bool terminate = HandleUpdate();
    if (terminate)
      return RETURN_CHARACTER;
  }
  assert(SPI::unread_character == true);

  // Read the buffer and mark as read.
  string input_buffer = SPI::input_buffer;
  SPI::unread_character = false;
  if (input_buffer.size() > 1) {
    // FATAL ERROR
  }
  return input_buffer[0];
}

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

void SetFont(FontSize size, Alignment al, int expected_width = 5) {
  font_size_ = size;
  alignment_ = al;
  if (alignment_ == ALIGNMENT_CENTER) {
    cursor_ = 0;
  } else if (alignment_ == ALIGNMENT_CENTER) {
    cursor_ = (MaxLength(font_size_) - expected_width) / 2;
  }
}

void SetCurrentLine(int line) {
  current_line_ = line;
}

void Display::DestinationPrompt() {
  bool last_failed = false;
  Point* destination_point = nullptr;

  SetFont(FONT_SIZE_MEDIUM, ALIGNMENT_LEFT);

  while (destination_point != nullptr) {
    ClearScreen();

    if (last_failed) {
      SetCurrentLine(1);
      PutString("Invalid location", true);
    }

    SetCurrentLine(2);
    PutString("Enter destination:", true);

    SetCurrentLine(3);
    destination_point = Map::StringToPoint(GetString());
    last_failed = destination_point == nullptr;

    if (Global::ShuttingDown)
      return;
  }
  display_thread_ = thread(Navigating);
}

void Display::Navigating() {
  bool done;
  while(not done) {
    {
      // Wait for user input and acquire lock.
      lock_guard<mutex>(dummy);
      while (update == DISPLAY_UPDATE_NONE) {
        display_update_pending.wait(dummy);
      }
    }
    if (update & DISPLAY_UPDATE_KEYPRESS) {
      SetFont(FONT_SIZE_MEDIUM, ALIGNMENT_LEFT);
      SetCurrentLine(2);
      PutString("What's up?", true);

      SetCurrentLine(3);
      PutString("A: Shutdown B: New Target C: Continue", true);
      char input = GetChar();
      if (input == 'A') {
        GLobal::ShutDown();
      if (input == 'B') {
        display_thread_ = thread(DestinationPrompt);
        return;
      } else {
        continue;
      }
    } else if (update & DISPLAY_UPDATE_ROUTE_CHANGE) {
      {
        lock_guard<mutex>(Navigation::buffer_mutex);

        // TODO: Read the Route and Point estimate buffers and mark as read.
      }
      //
    }

    // TODO: Handle data from buffer.
    // TODO: Set done when done.
  }
  display_thread_ = thread(Navigating);
}
