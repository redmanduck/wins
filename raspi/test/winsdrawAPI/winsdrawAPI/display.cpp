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

void Display::ClearLine(int line) {
    throw runtime_error("Not Implemented");
}

void Display::ClearScreen() {
    throw runtime_error("Not Implemented");
}

void Display::PutChar(char character) {
    throw runtime_error("Not Implemented");
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

char Display::GetChar(){
    throw runtime_error("Not Implemented");
    return 'a';
}

string Display::GetString() {
    int char_count = 0;
    string buffer;
    char last_character = '\0';
    while (char_count < MaxLength(FONT_SIZE_MEDIUM) or
           last_character == RETURN_CHARACTER) {
        last_character = this->GetChar();
        buffer += last_character;
    }
    return buffer;
}

void Display::PutLine(InkStyle istyle, int angle, int width){
    return;
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

void Display::WhereAmI() {
    throw runtime_error("Not Implemented");
}

void Display::Done() {
    GetChar();
    display_thread_ = thread(&Display::Menu, this);
}

void Display::Menu(){
    
}


Display::Display(Kyanvas * canv) {
    display_thread_ = thread(&Display::Menu, this);
    font_size_ = FONT_SIZE_MEDIUM;
    cursor_ = 0;
    current_line_ = 0;
    flushed_ = true;
}