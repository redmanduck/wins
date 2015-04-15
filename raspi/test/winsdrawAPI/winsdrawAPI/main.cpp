#include <iostream>
#include "display.h"
#include <cstdio>

using namespace std;


int main(int argc, const char * argv[]) {
    
    Display screen;
//    screen.ClearScreen();
//    screen.ShowMap("map_norm.csv");
    
    screen.gimmebitmap("yeahduck.bmp");
    
    return 0;
}
