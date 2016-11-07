#include <postscript.h>

#include "subpixel_bubbles.h"


void main(int argc, char *argv[])
{

  Point interp_point[2*(NU_ENO_MAXORDER+1)],tmp[2*(NU_ENO_MAXORDER+1)];
  Point grid_point,boundary_point;
  void *data;
  int i,j,k;
  int height=10,width=10;
  int interp_flag;

  if (argc != 7)
    error_msg("Usage: test_p boundary_point.x boundary_point.y grid_point.x grid_point.y height width");

  boundary_point.x=atof(argv[1]);
  boundary_point.y=atof(argv[2]);
  grid_point.x=atof(argv[3]);
  grid_point.y=atof(argv[4]);
  height=atoi(argv[5]);
  width=atoi(argv[6]);

  interp_flag = 
    compute_points_for_interpolation(grid_point,boundary_point,
                     interp_point, NU_ENO_MAXORDER+1,
                     height,width);
  k=0;
  for (i=0;i<2*(NU_ENO_MAXORDER+1);i++){
    if ( interp_point[i].x != INVALID_POINT &&
     interp_point[i].y != INVALID_POINT ){
      tmp[k].x=interp_point[i].x;
      tmp[k].y=interp_point[i].y;
      k++;
    }
  }

  init_draw_ps_original("interp.ps",10.0,height,width);
  draw_gridlines(0,0,height,width,9,data);
  for (i=0;i<k;i++)
    point_draw_ps(tmp[i].x,tmp[i].y,RED,data);

  point_draw_ps(boundary_point.x, boundary_point.y,BLUE,data);
  point_draw_ps(grid_point.x, grid_point.y,GREEN,data);


  close_draw_ps();


}




