#include <iostream>
#include "display.h"
#include <cstdio>
#include "glcdfont.c"

using namespace std;

void splashW(Kyanvas * canvas){
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(30,30), new IPoint(70,150));
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(70,150), new IPoint(120,30));
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(50+30,30), new IPoint(50+70,150));
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(50+70,150), new IPoint(50+120,30));
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(30 - 10,30), new IPoint(70-10,150));
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(70 - 10,150), new IPoint(120-10,30));
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(50+30-10,30), new IPoint(50+70-10,150));
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(50+70-10,150), new IPoint(50+120-10,30));
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(30 - 5,30), new IPoint(70-5,150));
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(70 - 5,150), new IPoint(120-5,30));
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(50+30-5,30), new IPoint(50+70-5,150));
    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(50+70-5,150), new IPoint(50+120-5,30));
}

int main(int argc, const char * argv[]) {
    
    Kyanvas * canvas = new Kyanvas(350, 200);
    int max_x = canvas->percentWidthToPixel(100) - 1;

//    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(0,30), new IPoint(max_x,30));
//    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(0,60), new IPoint(max_x,60));
//    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(0,90), new IPoint(max_x,90));
//    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(0,120), new IPoint(max_x,120));


//    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(0,199), new IPoint(100,0));
//    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(0,0), new IPoint(199,100));
//    canvas->drawLine(INK_STYLE_NORMAL, new IPoint(0,0), new IPoint(199,50));
//    canvas->drawCircle(INK_STYLE_NORMAL, 50, new IPoint(100,100));
//    canvas->drawRectangle(INK_STYLE_NORMAL, 10, 5, new IPoint(5,5));
//    canvas->printCanvas();
    canvas->toBitmap("jaychou.bmp");
    
    return 0;
}
