//
//  main.cpp
//  winsdrawAPI
//
//  Created by Pat Sabpisal on 3/27/15.
//  Copyright (c) 2015 Pat Sabpisal. All rights reserved.
//

#include <iostream>
#include "canvas.h"

using namespace std;
int main(int argc, const char * argv[]) {
    canvas * dutch = new canvas(128, 60);
    dutch->printCanvas();
    return 0;
}
