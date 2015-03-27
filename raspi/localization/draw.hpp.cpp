#include <string>

struct IPoint{
    int x;
    int y;
};

struct Font{
    int fontsize; //font size in px
    int charspacing; //character spacing in px
};

class canvas {
    public:
        canvas();
        canvas(const canvas& orig);
        virtual ~canvas();
        int width;
        int height;
        void drawText(std::string text, Font style, IPoint origin);
        void drawLine(int width, int thickness);
    private:

};
