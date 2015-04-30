#include <chrono>
#include <functional>
#include <stdexcept>

#ifndef ISOLATED_TEST
#include "common_utils.h"
#endif
#include "display.h"
#include "world.h"
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
  //system("clear");
  cout << "Screen refresh count = " << ++refresh_count << "\n";
  unsigned char r;
  unsigned char g;

  unsigned char b;
  unique_ptr<bitmap_image> image = glcd_.getImage();
  for (unsigned int y = 32; y > 0; --y) {
    for (unsigned int x = 0; x < 128; ++x) {
      image->get_pixel(x,  y*2-2, r, g, b); //64-
      bool down = r > 0;
      image->get_pixel(x, y*2-1, r, g, b);
      bool up = r > 0;
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
#endif
  flushed_ = true;
}


void Display::drawWorldCircle(uint8_t x0, uint8_t y0, uint8_t r,
                        uint8_t color) {

    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;

    setWorldPixel(x0, y0+r, color);
    setWorldPixel(x0, y0-r, color);
    setWorldPixel(x0+r, y0, color);
    setWorldPixel(x0-r, y0, color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        setWorldPixel(x0 + x, y0 + y, color);
        setWorldPixel(x0 - x, y0 + y, color);
        setWorldPixel(x0 + x, y0 - y, color);
        setWorldPixel(x0 - x, y0 - y, color);

        setWorldPixel(x0 + y, y0 + x, color);
        setWorldPixel(x0 - y, y0 + x, color);
        setWorldPixel(x0 + y, y0 - x, color);
        setWorldPixel(x0 - y, y0 - x, color);

    }



}

void Display::resetWorld(){
   //Highly inefficient function to reset map
   memcpy(WORLD, WORLD_MAP, 20910);
}

void Display::setWorldPixel(uint8_t x, uint8_t y, uint8_t color) {
    if ((x >= 170) || (y >= 128))
        return;

    if(color){
        WORLD[x+ (y/8)*170] |= (1<<(7-(y%8)));
    }else{
    	WORLD[x+ (y/8)*170] &= ~(1<<(7-(y%8)));
    }
    //WORLD[x+ (y/8)*170] &= ~(1<<(7-(y%8))); 

}

void Display::MapLoadWorld(string mapfile){
        //load background into big buffer
	//hard code for now
	//its in WORLD
}

int radii_ = 0;
void Display::MapDrawVisible(){
	lock_guard<mutex> lock(glcd_mutex);
	//this dump visible area into gcld
	//first extract the visible content from WORLD
	//use map_box as WORLD offset
	int W = 170;
	int world_offset = W*map_box_.second+map_box_.first;
//	memcpy(glcd_.st7565_buffer,&WORLD[world_offset], 128);
		
	//Draw indicator
	//indicator must be drawn in bottom layer
	resetWorld();

	if(radii_ > 3) radii_ = 0;
	setWorldPixel(map_indi_.first, map_indi_.second, 255);
	drawWorldCircle(map_indi_.first, map_indi_.second, radii_++,255);
	
	for(int i = 0; i < 8; i++){
	   int ac = i*128;//(128*7)-i*128;
	   
	   if(128 + world_offset + W*i >= 20910){
		cout << "World access out of bound!!!";
	   }  

	   memcpy(&glcd_.st7565_buffer[ac],&WORLD[world_offset + W*i], 128);
	   // Equation to reverse a byte :/
	   for(int j = 0; j < 128; j++){
              glcd_.st7565_buffer[ac+j] = ((glcd_.st7565_buffer[ac+j] * 0x0802LU & 0x22110LU) | ( glcd_.st7565_buffer[ac+j]* 0x8020LU & 0x88440LU)) *  0x10101LU >> 16;
	   }
	}

//	Draw indictor
//	if(radii_ > 5) radii_ = 0;
//	glcd_.setpixel(map_indi_.first, map_indi_.second, 255);
//	glcd_.drawcircle(map_indi_.first, map_indi_.second, radii_++, 255);
}

void Display::MapUpdateIndicator(Coord N, int rad){
	int DELTA = 30;
//	bool doUpdate = false;
	int new_box_x = map_box_.first;
	int new_box_y = map_box_.second;	

	//Shift Screen to reveal more	
	//If location N is beyond current map_box_ bound

	if(N.first >= map_box_.first + 128){
	 	cout << "Shifting X\n";	
		new_box_x += 20;
	}

	if(N.second >= map_box_.second + 64){
		cout << "Shifting Y\n";
		new_box_y += 20;
	}
          
	MapSetVisibleBound(new_box_x, new_box_y);
	
	//Update indicator position
	map_indi_.first = N.first;
	map_indi_.second = N.second;

	//note indi is within box
	//and box is what we draw
	
	cout << "Indicator: ";
	cout << map_indi_.first;
	cout << ", ";
	cout << map_indi_.second;
	cout << "\n";
}

void Display::MapSetVisibleBound(int x, int y){
	if(x >= (WORLD_WMAX - 128)){
		cout << "Normalizing X\n";
		x = WORLD_WMAX - 128;	
	}
	if(y >= (WORLD_HMAX - 64)){
		cout << "noramlizing Y\n";
		y = WORLD_HMAX - 64;	
	}
	cout <<"shifting bound " << x <<"," << y << "\n";
 	map_box_.first = x;
	map_box_.second = y;
}
Coord Display::ToScreenCoordinate(Coord sid){
	//TODO: turn scaled version of sid
	return sid;
}

Coord Display::ToSidCoordinate(Coord screen){
	//TODO: return reverse scaled version of screen
	return screen;
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
  this_thread::sleep_for(chrono::seconds(0));
//  MapSetVisibleBound(100, 0);
//  MapDrawVisible();
//  Flush();
 
  for(int i = 0; i < 64; i++){
//   MapSetVisibleBound(i*3,i);
   
   MapUpdateIndicator(Coord(i*5, 20), 4);
   MapDrawVisible();
   Flush();
     
   usleep(200000);
   //this_thread::sleep_for(chrono::seconds(1));
   if (system("CLS")) system("clear");  
  }

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
  //PutString("5. Map floor");
  //IncrmLine();
  IncrmLine();
  current_page_ = PAGE_MENU;

  while (true) {
    char option = GetChar();
    switch (option) {
      case '1': return PAGE_DESTINATION_PROMPT;
      case '2': return PAGE_NAVIGATING;
      case '3': return PAGE_CALBRATING;
      case '4': return PAGE_SHUT_DOWN;
      //case '5': return PAGE_MAP_SCAN;
      default : PutString("Enter a number in 1-4", true);
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

    ClearLine(3);
    SetCurrentLine(3);
    if (Navigation::TrySetDestinationFromCoords(GetStringAndEcho())) {
      break;
    }
    SetCurrentLine(1);
    PutString("Invalid location", true);
    this_thread::sleep_for(chrono::seconds(1));
    return PAGE_MENU;
  }
  return PAGE_NAVIGATING;
}

Page Display::Navigating() {
  ClearScreen();
  current_page_ = PAGE_NAVIGATING;
  bool inputing = false;
  SetFont(FONT_SIZE_MEDIUM, ALIGNMENT_LEFT);
  while(true) {
    auto result = Global::BlockForEvent(WINS_EVENT_ALL);
    auto events = result.events;
    if (events & WINS_EVENT_KEYPRESS) {
      if (not inputing) {
        inputing = true;
        ClearScreen();
        SetCurrentLine(2);
        PutString("What's up?", true);

        SetCurrentLine(3);
        PutString("1: Menu", true);
        IncrmLine();
        PutString("2: Continue", true);
        IncrmLine();
        GetChar();
      } else {
        char input = GetChar();
        if (input == '1') {
          Navigation::ResetDestination();
          return PAGE_MENU;
        } else {
          inputing = false;
          ClearScreen();
        }
      }
    }
    if (inputing) {
      continue;
    }
    if (events & WINS_EVENT_POS_CHANGE) {
      auto node = Location::GetCurrentNode();
      char buffer[50];
      sprintf(buffer, "PREV %3.3f, %3.3f", Location::prev_x, Location::prev_y);
      SetCurrentLine(1);
      PutString(buffer, true);
      sprintf(buffer, "IMU  %3.3f, %3.3f", Location::imu_x, Location::imu_y);
      SetCurrentLine(2);
      PutString(buffer, true);
      sprintf(buffer, "WIFI %3.3f, %3.3f", Location::wifi_x, Location::wifi_y);
      SetCurrentLine(3);
      PutString(buffer, true);
      sprintf(buffer, "KALM %3.3f, %3.3f", Location::kalman_x,
          Location::kalman_y);
      SetCurrentLine(4);
      PutString(buffer, true);

      if (node != nullptr) {
        auto point = node->point;
        SetCurrentLine(6);
        PutString("At (" + to_string((int)point->x) + ", " +
            to_string((int)point->y) + ")");
        auto node = Navigation::current_begin();
        if (node != Navigation::route_end()) {
          auto point = (*next(node))->point;
          IncrmLine();
          PutString("Go to (" + to_string((int)point->x) + ", " +
              to_string((int)point->y) + ")");
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

Page Display::MapScan() {
  while(true) {
    ClearScreen();
    SetCurrentLine(2);
    PutString("1: Add current");
    IncrmLine();
    PutString("location to map");
    IncrmLine();
    PutString("2: Quit mapping");
    IncrmLine();
    current_page_ = PAGE_MAP_SCAN;
    char option = GetChar();
    if (option == '2') {
      return PAGE_MENU;
    }

    ClearScreen();
    SetCurrentLine(3);
    PutString("Name of location");
    string name = GetStringAndEcho();

    ClearScreen();
    SetCurrentLine(3);
    PutString("Mapping...");
    IncrmLine();
    PutString("Please stay still");
    //Map::MapCurrentLocation(name);
  }
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
  //unique_ptr<uint8_t> buffer_copy(new uint8_t[1024]);
  //memcpy(buffer_copy.get(), glcd_.st7565_buffer, 1024);
  unique_ptr<uint8_t> buffer_copy = glcd_.getMappedBuffer();
  return buffer_copy;
}

Display& Display::GetInstance() {
  static Display display;
  return display;
}

}
