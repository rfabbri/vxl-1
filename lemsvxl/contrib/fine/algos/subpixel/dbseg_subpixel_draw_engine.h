#ifdef __cplusplus
extern "C" {
#endif


#define GREEN_COLOR 0
#define RED_COLOR 1
#define YELLOW_COLOR 2
#define BLUE_COLOR 3

#define PATCHSPAN(x) ((x) > 180. ? (x) - 360. : (((x) < -180.) ? (x) + 360. : x))
#define PATCHDEG(x) ((x) < 0 ? (x) + 360 : ((x) > 360) ? (x) - 360 : (x))
#define RAD2DEG(x) ((x) * 180 / M_PI)
#define DEG2RAD(x) ((x) * M_PI / 180)

void init_draw_ps( int width, int height,
          double scale,
          void* filename);

void close_draw_ps();

void set_color_ps(int color);


void line_draw_ps(double x1, double y1,
          double x2, double y2,
          int color);

void circle_draw_ps(double center_x, double center_y,
            double radius,
            int color);

void point_draw_ps(double x, double y, int color);
void arc_draw_ps(double angle1, double angle2,
         double center_x, double center_y,
         double radius,
         int color);

#ifdef __cplusplus
}
#endif

