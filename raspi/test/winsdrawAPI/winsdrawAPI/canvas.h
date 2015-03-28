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

enum InkStyle {
    INK_STYLE_THICK,
    INK_STYLE_NORMAL,
    INK_STYLE_DOUBLE
};

class Kyanvas {
public:
    Kyanvas(int w, int h);
    uint8_t * getRowData(int i);
    uint8_t ** getBitmap();
    void clear();
    int countRows();
    int countColumns();
    void printCanvas();
    void newLayer();
    void drawLine(InkStyle istyle, int width_px);
    void drawText(std::string text);
    int percentWidthToPixel(int percent);
    int percentHeightToPixel(int percent);
private:
    int width;
    int height;
    uint8_t ** bitmap;
};

#endif	/* CANVAS_H */

#endif /* defined(__winsdrawAPI__canvas__) */
