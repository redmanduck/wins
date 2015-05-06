#include <chrono>
#include <functional>
#include <stdexcept>

#include "common_utils.h"
#include "display.h"
#include "world.h"
#include "global.h"
#include "imu.h"
#include "keypad_handler.h"
#include "location.h"
#include "navigation.h"
#include "spi_manager.h"

namespace wins {

#define FLUSH_TO_SCREEN 1

using namespace std;

namespace {

std::ifstream::pos_type filesize(const char* filename)
{
  std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  return in.tellg();
}

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
   memcpy(WORLD, WORLD_MAP, WORLD_SIZE);
}

void Display::setWorldPixel(uint8_t x, uint8_t y, uint8_t color) {
    if ((x >= WORLD_WMAX) || (y >= WORLD_HMAX))
        return;

    if(color){
        WORLD[x+ (y/8)*WORLD_WMAX] ^= (1<<((y%8))); //xor , and no reverse
    }else{
    	WORLD[x+ (y/8)*WORLD_WMAX] &= ~(1<<((y%8)));
    }
}

void Display::MapLoadWorld(string mapfile){
	FILE * pFile;
        pFile = fopen (mapfile.c_str(), "rb");
        if (pFile==NULL) {fputs ("File error",stderr);
		cout << "Unable to open world: " + mapfile + ". Will crash.\n";
		throw;
	}
	//char wsize[8]; //size of world
	//char wmax[8]; //width of world
	//char hmax[8]; //height of world

	//fread(wsize, sizeof(char), 7, pFile);
	//fread(wmax, sizeof(char), 7, pFile);
	//fread(hmax, sizeof(char), 7, pFile);

//	cout << wsize;

/*	wsize[8] = 0x00;
	wmax[8] = 0x00;
	hmax[8] = 0x00;*/
	//int x = atoi(wsize);
//	int wm = atoi(wmax);
//	int hm = atoi(hmax);

	long unsigned int x, wm, hm;
	fscanf(pFile, "%07lu", &x);
	fscanf(pFile, "%07lu", &wm);
	fscanf(pFile, "%07lu", &hm);
	WORLD_SIZE = x;
	WORLD_HMAX = hm;
	WORLD_WMAX = wm;

	cout << WORLD_SIZE << " is world size\n";
	cout << WORLD_WMAX << " is world width\n";
	cout << WORLD_HMAX << " is world height\n";

   	WORLD = (uint8_t *)malloc(sizeof(uint8_t)*x);
	WORLD_MAP = (uint8_t *)malloc(sizeof(uint8_t)*x);
	size_t result = fread (WORLD,sizeof(uint8_t),x,pFile);
   	memcpy(WORLD_MAP, WORLD, x);
	if(result != (unsigned int)x){
		cout << "Unable to load world from " + mapfile + ". Will crash.\n";
		throw;
	}

	fclose(pFile);
}

int radii_ = 0;
void Display::MapDrawVisible(){
	lock_guard<mutex> lock(glcd_mutex);
	//this dump visible area into gcld
	//first extract the visible content from WORLD
	//use map_box as WORLD offset
	int W = WORLD_WMAX;
	int world_offset = W*map_box_.second+map_box_.first;
	cout << "World Offset Y: " << map_box_.second << "\n";
//	memcpy(glcd_.st7565_buffer,&WORLD[world_offset], 128);

	//Draw indicator
	//indicator must be drawn in bottom layer
	resetWorld();

	if(radii_ > 5) radii_ = 0;
	setWorldPixel(map_indi_.first, map_indi_.second, 255);
	drawWorldCircle(map_indi_.first, map_indi_.second, radii_++,255);

	for(int i = 0; i < 8; i++){
	   int ac = i*128;//(128*7)-i*128;

	   if(128 + world_offset + W*i >= WORLD_SIZE){
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

void Display::MapUpdateIndicator(Coord N){
//	bool doUpdate = false;
	int new_box_x = map_box_.first;
	int new_box_y = map_box_.second;

	int X_DELTA = 60;
	int Y_DELTA = 2;

	//Shift Screen to reveal more
	//If location N is beyond current map_box_ bound

	if(N.first >= map_box_.first + 128){
	 	cout << "Shifting X\n";
		new_box_x += X_DELTA;
	}

	if(N.second >= map_box_.second + 64){
		cout << "Shifting Y\n";
		new_box_y += Y_DELTA;
	}


	if(N.first <= map_box_.first){
	 	cout << "Shifting -X\n";
		new_box_x -= X_DELTA;
	}

	if(N.second <= map_box_.second){
		cout << "Shifting -Y\n";
		new_box_y -= Y_DELTA;
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
	if(y >= (WORLD_HMAX - 64)/8){
		cout << "noramlizing Y\n";
		y = (WORLD_HMAX - 64)/8;//(WORLD_HMAX - 64)/8;
	}
	cout <<"shifting bound " << x <<"," << y << "\n";
 	map_box_.first = x;
	map_box_.second = y;
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
  //Flush();
  this_thread::sleep_for(chrono::seconds(0));
  MapLoadWorld("worlds/EE.world");
  //  MapSetVisibleBound(100, 0);
  //  MapDrawVisible();
  //  Flush();

//  for(int f=0;f<10;f++){
//    MapUpdateIndicator(Coord(ORIGIN_X + 29*0.646913580247,ORIGIN_Y -2));
///    MapDrawVisible();
 //   Flush();
  //  usleep(100000);
//}
//
/*
  int corridor[16] = {1,4,7,10,13,0,3,6,9,12,15,2,5,8,11,14};
  std::vector<int> myvector (corridor, corridor+16);
  std::sort (myvector.begin(), myvector.begin()+16);

  //corridor 1 = 2.5 in x


  for(int i = 0; i < 16; i++){
   MapUpdateIndicator(Coord(ORIGIN_X+ myvector[i]*2.5,ORIGIN_Y));
//   MapUpdateIndicator(Coord(ORIGIN_X, ORIGIN_Y + i*3));

   MapDrawVisible();
   Flush();

   usleep(120000);
   //this_thread::sleep_for(chrono::seconds(1));
   if (system("CLS")) system("clear");
  }

  int corridor2[3] = {0,-1,-2};

  //corridor 2 =  2.45 in x, 4 in y
  for(int i = 0; i < 3; i++){
   MapUpdateIndicator(Coord(ORIGIN_X+ 16*2.45,ORIGIN_Y + corridor2[i]*4));

   MapDrawVisible();
   Flush();

   usleep(120000);
   //this_thread::sleep_for(chrono::seconds(1));
   if (system("CLS")) system("clear");
  }

  int corridor3[28] = {29 , 32 , 35 , 38 , 41 , 44 , 19 , 22 , 25 , 28 , 31 , 34 , 37 , 40 , 43 , 18 , 21 , 24 , 27 , 30 , 33 , 36 , 39 , 42 , 17 , 20 , 23 , 26};

   std::vector<int> mv (corridor3, corridor3+28);
   std::sort (mv.begin(), mv.begin()+28);
 //c3 : 3, 4.5
 // -2 , 24
  for(int i = 0; i < 28; i++){
   MapUpdateIndicator(Coord(ORIGIN_X + mv[i]*3 ,ORIGIN_Y + -2*4.5));
   cout << "pos.x : " << mv[i] << "\n";
   MapDrawVisible();
   Flush();

   usleep(190000);
   //this_thread::sleep_for(chrono::seconds(1));
   if (system("CLS")) system("clear");
  }

i*/
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
  PutString("5. Map floor");
  IncrmLine();
  PutString("6. Data dump");
  IncrmLine();
  current_page_ = PAGE_MENU;

  while (true) {
    char option = GetChar();
    switch (option) {
      case '1': return PAGE_DESTINATION_PROMPT;
      case '2': return PAGE_NAVIGATING;
      case '3': return PAGE_CALBRATING;
      case '4': return PAGE_SHUT_DOWN;
      case '5': return PAGE_MAP_SCAN;
      case '6': return PAGE_DATADUMP;
      default : PutString("Enter a number in 1-6", true);
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
  bool debug = false;
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
        if (debug) {
          PutString("3: Show debug", true);
        } else {
          PutString("3: Show map", true);
        }
        IncrmLine();
        GetChar();
      } else {
        char input = GetChar();
        if (input == '1') {
          Navigation::ResetDestination();
          return PAGE_MENU;
        } else if (input == '3') {
          debug = !debug;
          inputing = false;
          ClearScreen();
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
      if (node == NULL) {
        continue;
      }
      if (debug) {
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
      } else {
        cout <<  node->point->x << " sc: " << node->point->scale_x << "\n";
        cout << node->point->y << " sc: " << node->point->scale_y << "\n";
        MapUpdateIndicator(Coord(ORIGIN_X + node->point->y*node->point->scale_x ,ORIGIN_Y + node->point->x*node->point->scale_y));
        cout << "INDY:" << map_indi_.first << "," << map_indi_.second << "\n";
        MapDrawVisible();
      }

      Flush();
      if (node != nullptr) {
        auto point = node->point;
//        SetCurrentLine(6);
//        PutString("At (" + to_string((int)point->x) + ", " +
  //          to_string((int)point->y) + ")");
   //     IncrmLine();
//	PutString("Scale (" + to_string(point->scale_x) + ", " +
  //          to_string(point->scale_y) + ")");

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


string getNextSegmentName(){
   //For mapping mode
   //return next available segment filename
   system("mkdir /wifimap"); //Keep it simple stupid (KISS)
   FILE * max_p = popen("ls /wifimap  | sort -n -r | egrep -o '[0-9]+' | head -n 1", "r");
   char buffer[64];
   fgets(buffer, sizeof(buffer), max_p);
   pclose(max_p);
   return string(buffer);
}

bool MAP_SCANNING_ = false; //indicate map is scanning mode
int MAP_samples_ = 0;  	    //count number of sample
string MAP_FILENAME_ = "";  //map_file name
string MAP_record_ = ""; //map_file
ofstream MAP_fp_ ;
bool SKIP_SCAN = false;
void updateFilename(){
  string nxt = getNextSegmentName();
  int next = 0;
  try{
    next = stoi(nxt) + 1;
  }catch (...){
    cout << "Unable to determine filename. Crashing";
    throw;
    return;
  }
  MAP_FILENAME_ = to_string(next) + ".txt";
}

Page Display::MapScan() {

  if(MAP_FILENAME_ == ""){
  	updateFilename();
  }

  current_page_ = PAGE_MAP_SCAN;
  while (true) {
    ClearScreen();
    SetCurrentLine(1);
    cout << MAP_SCANNING_;
    PutString(MAP_FILENAME_);
    IncrmLine();
    if(!MAP_SCANNING_){
     IncrmLine();
     PutString("1. Start Scan");
     IncrmLine();
     PutString("2. New File");
     IncrmLine();
     PutString("3. Quit");
    }

    if(MAP_SCANNING_){
	bool skipflag = false;
	if(SKIP_SCAN){
		SKIP_SCAN = !SKIP_SCAN;
		skipflag = true;
	}
	for(int xx = 0; xx < 10; xx++){
	  if(skipflag) continue;
         vector<Result> r = Location::GetScans();
          MAP_samples_++;
          //This is one scan
       	  for(auto i = r.begin(); i != r.end(); i++ ){
	     MAP_record_ += i->name + "," + to_string(i->signal) +  "\n";
          }
 	  //EOS
  	  MAP_record_ += "-----------------------\n";

  	  ClearScreen();
	  PutString("Collecting..");
  	  IncrmLine();
	  PutString("# Sample: " + to_string(MAP_samples_));
     	  IncrmLine();

      }
	  ClearScreen();
	  PutString("# Sample: " + to_string(MAP_samples_));
     	  IncrmLine();
          PutString("1. Done");
          IncrmLine();
          PutString("3. Delimit");

    }

    char option = GetChar();
    if (option == '1') {
      MAP_SCANNING_ = !MAP_SCANNING_;
      MAP_samples_ = 0;

      if(!MAP_SCANNING_){
    	ClearScreen();
	SetCurrentLine(3);
	PutString("Saved to " + MAP_FILENAME_  + "!");
	MAP_fp_.open("/wifimap/" + MAP_FILENAME_, ios::out);
	MAP_fp_ << MAP_FILENAME_ << "\n";
        MAP_fp_ << MAP_record_;
	MAP_fp_.close();
	usleep(1000000);
     }

      MAP_record_ = "";
      MAP_FILENAME_ = "";
      return PAGE_MAP_SCAN;
    } else if(option == '2'){
	updateFilename();
    }else if(option == '3' && !MAP_SCANNING_){
      MAP_FILENAME_ = "";
      MAP_samples_ = 0;
      return PAGE_MENU;
    //}else if(option == '2' && MAP_SCANNING_){
    //   continue;
    }else if(option == '3' && MAP_SCANNING_){
     ClearScreen();
     PutString("Confirm delim? (5=Y)");
     IncrmLine();
     Flush();
     char option2 = GetChar();
     if(option2 == '5'){
      MAP_record_ += "========== [FLAGGED] =============\n";
	cout << "MPU6050" << MAP_record_;
     }else{
	SKIP_SCAN = true;
     }
    }
  }
}

Page Display::DataDump() {
  ClearScreen();
  SetCurrentLine(4);
  PutString("Start Dump?", true);
  current_page_ = PAGE_DATADUMP;

  std::time_t rawtime;
  std::tm* timeinfo;
  char buffer [80];
  std::time(&rawtime);
  timeinfo = std::localtime(&rawtime);
  std::strftime(buffer,80,"%m-%d-%H-%M-%S.txt",timeinfo);
  Global::DumpFile = string(buffer);
  cout << buffer << "\n";
  SetCurrentLine(3);
  PutString(buffer);

  SetCurrentLine(4);
  GetChar();
  ofstream dumpfile(Global::DumpFile, ofstream::trunc);
  auto north_quat = Imu::GetNorthQuat();
  dumpfile << "North," << north_quat.w() << "," << north_quat.x() << "," <<
      north_quat.y() << "," << north_quat.z() << "\n";
  dumpfile.close();
  Global::DataDump = true;
  PutString("Press key to stop", true);
  GetChar();
  Global::DataDump = false;
  PutString("Data dump complete", true);
  IncrmLine();
  PutString(to_string(filesize(buffer)));
  this_thread::sleep_for(chrono::seconds(1));
  return PAGE_MENU;
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
    case PAGE_MAP_SCAN:		    return MapScan();
    case PAGE_NAVIGATING:           return Navigating();
    case PAGE_DESTINATION_PROMPT:   return DestinationPrompt();
    case PAGE_SHUT_DOWN:            return ShutDown();
    case PAGE_DONE:                 return Done();
    case PAGE_DATADUMP:             return DataDump();
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
