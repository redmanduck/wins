#include <iostream>
#include "display.h"
#include <cstdio>

using namespace std;


int main(int argc, const char * argv[]) {
    
    Display screen;
    screen.gimmebitmap("yeahduck.bmp");
    
//    uint8_t st7565_buffer[1024] =  screen.gimme_st7565_buffer();
    return 0;
}
