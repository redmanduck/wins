//
//  main.cpp
//  winsdrawAPI
//
//  Created by Pat Sabpisal on 3/27/15.
//  Copyright (c) 2015 Pat Sabpisal. All rights reserved.
//

#include <iostream>
#include "display.h"

using namespace std;
int main(int argc, const char * argv[]) {
    Kyanvas * canvas = new Kyanvas(60, 15);
    canvas->printCanvas();
    
    
    return 0;
}
