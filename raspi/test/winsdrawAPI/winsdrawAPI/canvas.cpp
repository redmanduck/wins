#include "canvas.h"
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
#include <iostream>
#include <math.h>

Kyanvas::Kyanvas(int w, int h) {
    //Initialize canvas
    this->height = h;
    this->width = w;
    this->bitmap = new uint8_t*[w];
    for(int i = 0; i < w; i++ ){
        this->bitmap[i] = new uint8_t[h];
        for(int j = 0; j < h; j++){
            this->bitmap[i][j] = 0;
        }
    }
//    std::cout << glcdfont[0];
}

void Kyanvas::printCanvas(){
    std::cout << this->width << "x" << this->height << std::endl;
    for(int i = 0; i < this->height; i++ ){
        for(int j = 0; j < this->width; j++){
            std::cout << (this->bitmap[j][i] == 1 ? '_' : ' ');
        }
        std::cout << "\n";
    }
}

/*
 * Bresenham's line algorithm
 */
void Kyanvas::drawLine(InkStyle istyle, IPoint * start, IPoint * end){
    
    int x0 = start->x;
    int y0 = start->y;
    int dx = abs(end->x-x0), sx = x0<end->x ? 1 : -1;
    int dy = abs(end->y-y0), sy = y0<end->y ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;
    
    for(;;){
        putPixel(x0,y0);
        if (x0==end->x && y0==end->y) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

/*
 * Draw filled circle
 */
void Kyanvas::drawCircle(InkStyle istyle, int radius, IPoint * centre){
    int x, y;
    
    for (y = -radius; y <= radius; y++)
        for (x = -radius; x <= radius; x++)
            if ((x * x) + (y * y) <= (radius * radius))
                putPixel(x + centre->x, y + centre->y);

}

/*
 * draw rectangle
 */
void Kyanvas::drawRectangle(InkStyle istyle, int width_px, int height_px, IPoint * origin){
    IPoint * bottomleft = new IPoint(origin->x, origin->y + height_px);
    IPoint * bottomright = new IPoint(origin->x + width_px, origin->y + height_px);
    IPoint * topright = new IPoint(origin->x + width_px, origin->y);
   
    drawLine(istyle, origin, bottomleft);
    drawLine(istyle, origin, topright);
    drawLine(istyle, bottomleft, bottomright);
    drawLine(istyle, bottomright, topright);
}

int Kyanvas::percentWidthToPixel(int percent){
    return int(percent*width/100);
}

void Kyanvas::toBitmap(std::string filename){
    bitmap_image image(this->width,this->height);
    
    for (unsigned int x = 0; x < this->width; ++x)
    {
        for (unsigned int y = 0; y < this->height; ++y)
        {
            if(this->bitmap[x][y] != 1){
                continue;
            }
            image.set_pixel(x,y,255,255,255);
        }
    }
    
    
    image.save_image(filename);
}

int Kyanvas::countRows(){
    return this->height;
}

int Kyanvas::countColumns(){
    return this->width;
}

void Kyanvas::putPixel(int x, int y){
    this->bitmap[x][y] = 1;
}
void Kyanvas::clearPixel(int x, int y){
    this->bitmap[x][y] = 0;
}

