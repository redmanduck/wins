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

canvas::canvas(int w, int h) {
    //Initialize canvas
    this->height = h;
    this->width = w;
    this->bitmap = new uint8_t*[w];
    for(int i = 0; i < w; i++ ){
        this->bitmap[i] = new uint8_t[h];
        for(int j = 0; j < h; j++){
            this->bitmap[j] = 0;
        }
    }
}

void canvas::printCanvas(){
    for(int i = 0; i < this->width; i++ ){
        for(int j = 0; j < this->height; j++){
            std::cout << this->bitmap[i][j];
        }
        std::cout << "\n";
    }
}
int canvas::countRows(){
    return this->height;
}
int canvas::countColumns(){
    return this->width;
}
