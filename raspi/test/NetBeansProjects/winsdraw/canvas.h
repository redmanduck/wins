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

