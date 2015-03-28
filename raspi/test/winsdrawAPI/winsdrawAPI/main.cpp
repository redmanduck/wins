#include <iostream>
#include "display.h"

using namespace std;
int main(int argc, const char * argv[]) {
    Kyanvas * canvas = new Kyanvas(60, 20);
//    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(0,0), new IPoint(59,19));
//    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(0,19), new IPoint(59,0));
//    canvas->drawCircle(INK_STYLE_NORMAL, 5, new IPoint(20,10));
    canvas->drawRectangle(INK_STYLE_NORMAL, 10, 5, new IPoint(5,5));
    canvas->printCanvas();
    
    return 0;
}
