#ifdef __cplusplus
extern "C" {
#endif

#ifndef _DRAW_PRIMITIVES_H_
#define _DRAW_PRIMITIVES_H_

#define PATCHSPAN(x) ((x) > 180. ? (x) - 360. : (((x) < -180.) ? (x) + 360. : x))
#define PATCHDEG(x) ((x) < 0 ? (x) + 360 : ((x) > 360) ? (x) - 360 : (x))
#define RAD2DEG(x) ((x) * 180 / M_PI)
#define DEG2RAD(x) ((x) * M_PI / 180)

#define RED 0
#define GREEN 1
#define BLUE 2
#define GREY 3 
#define PURPLE 4
#define LIGHT_BLUE 5
#define VIOLET 6
#define DARK_GREEN 7
#define BROWN 8
#define YELLOW 9
#define LIGHT_GREEN 10
#define BLACK 12
#define WHITE 11

typedef struct {
  double x1,y1,x2,y2,x3,y3,x4,y4;
}Polygon1;


void init_draw_ps(char *filename, double scale,int width, int height);
void close_draw_ps();
void set_color_ps(int color, void* data);
void adjust_line_thickness(double size, void *data);
void point_draw_ps(double x, double y, int color, void* data);
void line_draw_ps(double x1, double y1,double x2, double y2,
            int color, void* data);
void cross_draw_ps(double x, double y, int color, void* data);
void draw_vector(double x, double y, double size, double direction, int color, void *data); 
void draw_arrow(double x, double y, double size, double direction, int color, void *data);
void draw_arrow(double x, double y, double size, double direction, int color, void *data); 
void circle_draw_ps(double center_x, double center_y,double radius,
            int color, void* data);
void arc_draw_ps(double angle1, double angle2, double center_x, 
         double center_y, double radius, int color,void* data);

void draw_gridlines(double xstart, double ystart, int width, int height, int  color, void *data);
void draw_gridlines_skip(int xstart, int ystart, int xstep, int ystep,int width, 
             int height, int color, void *data);
void init_draw_ps_original(char *filename, double scale,int width, int height);

void draw_directional_vectors(float y0, float x0, int dir, float size, int color,int init);
void draw_polygon_ps(Polygon1 *pol, double fill, void *data);
void draw_rectangle_ps(double x, double y, double width, double height, int fill, 
               double color, void *data);

double angular_addition_ps(double angle1, double angle2);

#endif /* DRAW_PRIMITIVES */

#ifdef __cplusplus
}
#endif

