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
    
    int dx = abs(end->x-start->x), sx = start->x<end->x ? 1 : -1;
    int dy = abs(end->y-start->y), sy = start->y<end->y ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;
    
    for(;;){
        putPixel(start->x,start->y);
        if (start->x==end->x && start->y==end->y) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; start->x += sx; }
        if (e2 < dy) { err += dx; start->y += sy; }
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

int Kyanvas::percentWidthToPixel(int percent){
    return int(percent*width/100);
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

