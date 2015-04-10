#ifndef DISPLAY_H
#define DISPLAY_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
//
//#include "map.h"
//
#include "ST7565.h"

using namespace std;

enum DisplayUpdate {
    DISPLAY_UPDATE_NONE               = 00,
    DISPLAY_UPDATE_NAV_CHANGE         = 01,
    DISPLAY_UPDATE_DEST_REACHED       = 02,
    DISPLAY_UPDATE_KEYPRESS           = 04,
    DISPLAY_UPDATE_BATTERY_LOW        = 010,
    DISPLAY_UPDATE_BATTERY_CRITICAL   = 020,
    DISPLAY_UPDATE_ALL                = 077
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
    thread display_thread_;
    
    condition_variable display_update_pending_;
    mutex update_mutex_;
    DisplayUpdate update_;
    
    FontSize font_size_;
    Alignment alignment_;
    int cursor_;
    int current_line_;
    bool flushed_;
    
    void BlockForUpdate(DisplayUpdate type);
    void ClearLine(int line);
    void IncrmLine();
    void ClearScreen();
    void PutChar(char character);
    char GetChar();
    void PutString(string s, bool clear=false);
    string GetString();
    void Flush();
    void SetFont(FontSize size, Alignment al, int expected_width = 5);
    void SetCurrentLine(int line);
    
    
public:
    void SetUpdateFlag(DisplayUpdate flag);
    void gimmebitmap(string saveas);
    uint8_t * gimme_st7565_buffer();

    void Menu();
    void WhereAmI();
//    void DestinationPrompt(NavMode mode);
//    void Navigating(NavMode mode);
    void Done();
    
    Display();
    
    // Prevent copying.
    Display(const Display& other) = delete;
};

#endif
