#include <chrono>
#include <functional>
#include <stdexcept>
#include <string>
//#include "common_utils.h"
#include "display.h"

//#include "navigation.h"
//#include "spi_manager.h"

using namespace std;

char RETURN_CHARACTER = '\0';
ST7565 glcd(0);

int MaxLength(FontSize size) {
    if(size == FONT_SIZE_MEDIUM){
        return (int)(128/6);
    }
    throw runtime_error("Not Implemented");
}

int MaxLines(FontSize size) {
    if(size == FONT_SIZE_MEDIUM){
         return 8;
    }
    throw runtime_error("Not Implemented");
    return 0;
}

//void Display::BlockForUpdate(DisplayUpdate type) {
//    unique_lock<mutex> lock(update_mutex_);
//    while (not update_ & type) {
//        display_update_pending_.wait(lock);
//        if (update_ & DISPLAY_UPDATE_BATTERY_CRITICAL) {
//            Global::ShutDown();
//        }
//    }
//}


void Display::ShowMap(string path_to_map){
    
    glcd.drawpixel(75,30, BLACK);
    glcd.drawpixel(82,30, BLACK);
    glcd.drawpixel(88,30, BLACK);
    glcd.drawpixel(95,30, BLACK);
    glcd.drawpixel(102,30, BLACK);
    glcd.drawpixel(108,30, BLACK);
    glcd.drawpixel(13,33, BLACK);
    glcd.drawpixel(19,33, BLACK);
    glcd.drawpixel(26,33, BLACK);
    glcd.drawpixel(33,33, BLACK);
    glcd.drawpixel(39,33, BLACK);
    glcd.drawpixel(46,33, BLACK);
    glcd.drawpixel(46,32, BLACK);
    glcd.drawpixel(46,30, BLACK);
    glcd.drawpixel(53,30, BLACK);
    glcd.drawpixel(59,30, BLACK);
    glcd.drawpixel(66,30, BLACK);
    glcd.drawpixel(73,30, BLACK);
    glcd.drawpixel(79,30, BLACK);
    glcd.drawpixel(86,30, BLACK);
    glcd.drawpixel(93,30, BLACK);
    glcd.drawpixel(99,30, BLACK);
    glcd.drawpixel(106,30, BLACK);
    glcd.drawpixel(10,33, BLACK);
    glcd.drawpixel(17,33, BLACK);
    glcd.drawpixel(24,33, BLACK);
    glcd.drawpixel(30,33, BLACK);
    glcd.drawpixel(37,33, BLACK);
    glcd.drawpixel(44,33, BLACK);
    glcd.drawpixel(50,30, BLACK);
    glcd.drawpixel(57,30, BLACK);
    glcd.drawpixel(64,30, BLACK);
    glcd.drawpixel(70,30, BLACK);
    glcd.drawpixel(77,30, BLACK);
    glcd.drawpixel(84,30, BLACK);
    glcd.drawpixel(90,30, BLACK);
    glcd.drawpixel(97,30, BLACK);
    glcd.drawpixel(104,30, BLACK);
    glcd.drawpixel(110,30, BLACK);
    glcd.drawpixel(110,29, BLACK);
    glcd.drawpixel(15,33, BLACK);
    glcd.drawpixel(110,27, BLACK);
    glcd.drawpixel(110,26, BLACK);
    glcd.drawpixel(22,33, BLACK);
    glcd.drawpixel(110,24, BLACK);
    glcd.drawpixel(110,23, BLACK);
    glcd.drawpixel(28,33, BLACK);
    glcd.drawpixel(110,21, BLACK);
    glcd.drawpixel(110,20, BLACK);
    glcd.drawpixel(35,33, BLACK);
    glcd.drawpixel(110,18, BLACK);
    glcd.drawpixel(110,17, BLACK);
    glcd.drawpixel(42,33, BLACK);
    glcd.drawpixel(48,30, BLACK);
    glcd.drawpixel(55,30, BLACK);
    glcd.drawpixel(62,30, BLACK);
    glcd.drawpixel(68,30, BLACK);
    glcd.drawpixel(39,60, BLACK);
    glcd.drawpixel(46,60, BLACK);
    glcd.drawpixel(53,60, BLACK);
    glcd.drawpixel(59,60, BLACK);
    glcd.drawpixel(66,60, BLACK);
    glcd.drawpixel(73,60, BLACK);
    glcd.drawpixel(79,60, BLACK);
    glcd.drawpixel(10,60, BLACK);
    glcd.drawpixel(10,59, BLACK);
    glcd.drawpixel(10,57, BLACK);
    glcd.drawpixel(17,60, BLACK);
    glcd.drawpixel(10,56, BLACK);
    glcd.drawpixel(10,54, BLACK);
    glcd.drawpixel(24,60, BLACK);
    glcd.drawpixel(10,53, BLACK);
    glcd.drawpixel(10,51, BLACK);
    glcd.drawpixel(30,60, BLACK);
    glcd.drawpixel(10,50, BLACK);
    glcd.drawpixel(10,48, BLACK);
    glcd.drawpixel(37,60, BLACK);
    glcd.drawpixel(10,47, BLACK);
    glcd.drawpixel(10,45, BLACK);
    glcd.drawpixel(44,60, BLACK);
    glcd.drawpixel(10,44, BLACK);
    glcd.drawpixel(50,60, BLACK);
    glcd.drawpixel(10,42, BLACK);
    glcd.drawpixel(10,41, BLACK);
    glcd.drawpixel(57,60, BLACK);
    glcd.drawpixel(10,39, BLACK);
    glcd.drawpixel(10,38, BLACK);
    glcd.drawpixel(64,60, BLACK);
    glcd.drawpixel(10,36, BLACK);
    glcd.drawpixel(10,35, BLACK);
    glcd.drawpixel(70,60, BLACK);
    glcd.drawpixel(10,33, BLACK);
    glcd.drawpixel(77,60, BLACK);
    glcd.drawpixel(84,60, BLACK);
    glcd.drawpixel(84,59, BLACK);
    glcd.drawpixel(84,57, BLACK);
    glcd.drawpixel(84,56, BLACK);
    glcd.drawpixel(84,54, BLACK);
    glcd.drawpixel(84,53, BLACK);
    glcd.drawpixel(84,51, BLACK);
    glcd.drawpixel(84,50, BLACK);
    glcd.drawpixel(84,48, BLACK);
    glcd.drawpixel(84,47, BLACK);
    glcd.drawpixel(84,45, BLACK);
    glcd.drawpixel(84,44, BLACK);
    glcd.drawpixel(15,60, BLACK);
    glcd.drawpixel(84,42, BLACK);
    glcd.drawpixel(84,41, BLACK);
    glcd.drawpixel(22,60, BLACK);
    glcd.drawpixel(84,39, BLACK);
    glcd.drawpixel(28,60, BLACK);
    glcd.drawpixel(35,60, BLACK);
    glcd.drawpixel(42,60, BLACK);
    glcd.drawpixel(48,60, BLACK);
    glcd.drawpixel(55,60, BLACK);
    glcd.drawpixel(62,60, BLACK);
    glcd.drawpixel(68,60, BLACK);
    glcd.drawpixel(75,60, BLACK);
    glcd.drawpixel(82,60, BLACK);
    glcd.drawpixel(13,60, BLACK);
    glcd.drawpixel(19,60, BLACK);
    glcd.drawpixel(26,60, BLACK);
    glcd.drawpixel(33,60, BLACK);

    glcd.drawstring(10, 1, "EE Basement");
}

void Display::ClearLine(int line) {
    glcd.fillrect(0, line*8 , 128, 8, 0);
}

void Display::ClearScreen() {
    glcd.clear();
}

//void Display::PutChar(char character) {
//    throw runtime_error("Not Implemented");
//}

char Display::GetChar() {
//    Flush();
//    BlockForUpdate(DISPLAY_UPDATE_KEYPRESS);
//    assert(SPI::input_buffer.length() > 0);
//    
//    // Acquire lock to read buffer till end of method.
//    lock_guard<mutex> lock(SPI::buffer_mutex);
//    
//    // Read the buffer and mark as read.
//    char input_char = SPI::input_buffer[0];
//    SPI::input_buffer.erase(0, 1);
//    
//    return input_char;
    return 'c';
}

void Display::PutString(string s, bool clear) {
    if (clear) {
        ClearLine(current_line_);
        cursor_ = 0;
    }
    glcd.drawstring_P(cursor_, current_line_, s.c_str());
    cursor_ += s.length();
}

string Display::GetString() {
    int char_count = 0;
    string buffer;
    char last_character = '\0';
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
    if(line == 0){
        throw runtime_error("You arent allowed to set the top line. Reserved space for PIC!");
    }
    current_line_ = line;
    cursor_ = 0;
    if (line > MaxLines(font_size_)) {
        current_line_ = MaxLines(font_size_) - 1;
    }
}

void Display::IncrmLine(){
    SetCurrentLine(++current_line_);
}

void Display::SetUpdateFlag(DisplayUpdate flag) {
    throw runtime_error("Not Implemented");
}

void Display::Menu() {
    
    ClearScreen();
    
    PutString("1. NAVIGATION");
    IncrmLine();
    IncrmLine();
    PutString("2. WHERE AM I?");
    IncrmLine();
    IncrmLine();
    PutString("3. SHUT DOWN");
    
//    ClearLine(2);
    
    //TODO: construct canvas for menu
//    char option = GetChar();
//    switch (option) {
//        case '1': display_thread_ =
//            thread(&Display::DestinationPrompt, this, NAV_MODE_ROUTE); break;
//        case '2': display_thread_ =
//            thread(&Display::DestinationPrompt, this, NAV_MODE_LOCATE); break;
//        case '3': Global::ShutDown(); break;
//    }
}

void Display::WhereAmI() {
    throw runtime_error("Not Implemented");
}


//void Display::DestinationPrompt(NavMode mode) {
//    SetFont(FONT_SIZE_MEDIUM, ALIGNMENT_LEFT);
//    
//    while (true) {
//        ClearScreen();
//        
//        SetCurrentLine(2);
//        PutString("Enter destination:", true);
//        
//        SetCurrentLine(3);
//        if (Navigation::TrySetDestinationFromCoords(GetString())) {
//            SetCurrentLine(1);
//            PutString("Invalid location", true);
//            break;
//        }
//    }
//    display_thread_ = thread(&Display::Navigating, this, mode);
//}

//void Display::Navigating(NavMode mode) {
//    while(true) {
//        if (update_ == DISPLAY_UPDATE_NONE) {
//            BlockForUpdate(DISPLAY_UPDATE_ALL);
//        }
//        if (update_ & DISPLAY_UPDATE_KEYPRESS) {
//            SetFont(FONT_SIZE_MEDIUM, ALIGNMENT_LEFT);
//            SetCurrentLine(2);
//            PutString("What's up?", true);
//            
//            SetCurrentLine(3);
//            PutString("1: Menu 2: Continue", true);
//            char input = GetChar();
//            if (input == '1') {
//                display_thread_ = thread(&Display::Menu, this);
//                return;
//            } else {
//                continue;
//            }
//        }
//        if (update_ & DISPLAY_UPDATE_NAV_CHANGE) {
//            lock_guard<mutex> lock(Navigation::route_mutex);
//            
//            // TODO: Read the Route and Point estimate buffers and mark as read.
//        }
//        if (update_ & DISPLAY_UPDATE_DEST_REACHED) {
//            display_thread_ = thread(&Display::Done, this);
//            return;
//        }
//    }
//}

void Display::Done() {
    GetChar();
    display_thread_ = thread(&Display::Menu, this);
}

uint8_t * Display::gimme_st7565_buffer(){
    return glcd.get_st7565_buffer();
}


Display::Display() {
    font_size_ = FONT_SIZE_MEDIUM;
    cursor_ = 0;
    current_line_ = 1;
    flushed_ = true;
    
    Menu();
    
    
    //display_thread_ = thread(&Display::Menu, this);

}


void Display::gimmebitmap(string saveas){
    glcd.savebitmap(saveas);
}
