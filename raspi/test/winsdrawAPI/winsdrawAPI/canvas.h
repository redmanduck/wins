//
//  canvas.h
//  winsdrawAPI
//
//  Created by Pat Sabpisal on 3/27/15.
//  Copyright (c) 2015 Pat Sabpisal. All rights reserved.
//

#ifndef __winsdrawAPI__canvas__
#define __winsdrawAPI__canvas__

#include <stdio.h>
#include <string>

#ifndef CANVAS_H
#define	CANVAS_H

struct IPoint{
    int x;
    int y;
};

class canvas {
public:
    canvas(int w, int h);
    uint8_t * getRowData(int i);
    uint8_t ** getBitmap();
    void clear();
    int countRows();
    int countColumns();
    void printCanvas();
private:
    int width;
    int height;
    uint8_t ** bitmap;
};

#endif	/* CANVAS_H */



#endif /* defined(__winsdrawAPI__canvas__) */
