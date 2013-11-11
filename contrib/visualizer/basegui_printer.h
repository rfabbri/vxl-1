#ifndef PRINTER_H_INCLUDED
#define PRINTER_H_INCLUDED

class GraphicsView;
class GraphicsNode;

// width is in inches
// thicken uniformly scales the line widths, point sizes, and text sizes, for
// making exaggerated plots.
// border is in inches, specifies a border to add around the printout

// the image is printed with the same rotation & orientation as the GraphicsView,
// however translation and scale are ignored.
// this prints the _whole_ canvas (including offscreen stuff)
void print(const char *file, GraphicsView *c, double width, GraphicsNode *object_to_print = 0, double thicken = 1.0,
    double border = 6./72.);

// print only the rectangle specified. x1, y1, x2, y2 are in screen coordinates (pixels)
void print(const char *file, GraphicsView *c, double width,
    int x1, int y1, int x2, int y2, GraphicsNode *object_to_print = 0, double thicken = 1.0);

#endif
