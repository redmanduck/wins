#include <iostream>
#include "display.h"

using namespace std;
int main(int argc, const char * argv[]) {
    Kyanvas * canvas = new Kyanvas(60, 20);
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(0,0), new IPoint(59,19));
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(0,19), new IPoint(59,0));
    canvas->printCanvas();
    
    return 0;
}
