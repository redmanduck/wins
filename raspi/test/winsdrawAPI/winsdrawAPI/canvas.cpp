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
            std::cout << int(this->bitmap[j][i]);
        }
        std::cout << "\n";
    }
}

int Kyanvas::countRows(){
    return this->height;
}

int Kyanvas::countColumns(){
    return this->width;
}
