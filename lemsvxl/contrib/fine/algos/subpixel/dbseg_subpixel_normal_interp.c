#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dbseg_subpixel_postscript.h"
#include "dbseg_subpixel_1d_eno_types.h"
#include "dbseg_subpixel_non-uniform-eno.h"
#include "dbseg_subpixel_tbs-utils.h"
#include "dbseg_subpixel_SPCedtGeno.h"
#include "dbseg_subpixel_postscript.h"

#include "dbseg_subpixel_subpixel_bubbles.h"

/**************************************************************************
  Description: Function that finds the gradient at a grid point. It uses
               central differences for finding the derivatives.
  Inputs :  boundary point - The point on the boundary.
        enoimage - ENO interpolated image (along normal grid lines)
  Outputs: grad_x,grad_y - gradients in the X & Y directions respectively
  Return :  gradient at the boundary point        

**************************************************************************/

double find_grid_gradient(Point boundary_point, ENO_Interval_Image_t *enoimage, double *grad_x, double *grad_y)
{
  Point xp,yp,xm,ym;
  double xgrad,ygrad;
  double xpv,ypv,xmv,ymv;
  double force;

  xm.x=boundary_point.x-1;
  xm.y=boundary_point.y;
  xp.x=boundary_point.x+1;
  xp.y=boundary_point.y;
  xmv=find_correct_eno_interpolation(enoimage,xm);
  xpv=find_correct_eno_interpolation(enoimage,xp);

  if (xmv != IMPOSSIBLE_VALUE && xpv != IMPOSSIBLE_VALUE)
    xgrad = xpv-xmv;
  else
    xgrad = 0.0;

  ym.x=boundary_point.x;
  ym.y=boundary_point.y-1;
  yp.x=boundary_point.x;
  yp.y=boundary_point.y+1;
  ymv=find_correct_eno_interpolation(enoimage,xm);
  ypv=find_correct_eno_interpolation(enoimage,xp);

  if (xmv != IMPOSSIBLE_VALUE && xpv != IMPOSSIBLE_VALUE)
    ygrad = ypv-ymv;
  else
    ygrad = 0.0;
  *grad_x = xgrad;
  *grad_y = ygrad;

  force=sqrt(xgrad*xgrad+ygrad*ygrad);
  return force;
}

/**************************************************************************
  Description: Function that finds the ENO interpolation value and the
               first ENO derivative at the boundary point. ENO is done
           along a line drawn from the grid point to the boundary 
           point.
  Inputs :  grid point - Grid location
            boundary point - The closest point on the boundary.
        enoimage - ENO interpolated image (along normal grid lines)
   Outputs : first_der - the value of the 1st derivative at the boundary point
             second_der - the value of the 1st derivative at the boundary point
         interp_value - ENO interpolated value at the boundary point

**************************************************************************/
void compute_subpixel_interp_ders(Point grid_point, Point boundary_point,
                  ENO_Interval_Image_t *enoimage, double *first_der,
                  double *second_der, double *interp_value)
{

  Point interp_point[2*(NU_ENO_MAXORDER+1)],tmp[2*(NU_ENO_MAXORDER+1)];
  Point new_pt;
  NU_ENO_Interval ei[2*(NU_ENO_MAXORDER+1)];
  void *data;
  double x[2*(NU_ENO_MAXORDER+1)],y[2*(NU_ENO_MAXORDER+1)];
  double boundary_loc,force;
  double x_grad,y_grad;
  int interp_flag,i,pos,ix,iy,k;
  int height,width,boundary_idx;
  int verbose = 1;

  height=enoimage->height;
  width=enoimage->width;

  /*If the boundary point and the grid point coincide, then 
    simply take the gradient */
  if (almost_equal(grid_point.x,boundary_point.x) &&
      almost_equal(grid_point.y,boundary_point.y) ){
    *first_der = find_grid_gradient(boundary_point,enoimage,&x_grad,&y_grad);
    *interp_value = find_correct_eno_interpolation(enoimage,boundary_point); 
    return;
  }

  interp_flag = 
    compute_points_for_interpolation(grid_point,boundary_point,
                     interp_point, NU_ENO_MAXORDER+1,
                     height,width);
  /*Find enough interpolation points */
  if (interp_flag == 0){
    /* printf("Enough interpolation points could not be found F=%d \n",interp_flag); */
    new_pt.x=(double)floor(boundary_point.x);
    new_pt.y=(double)floor(boundary_point.y);
    *first_der = find_grid_gradient(new_pt,enoimage,&x_grad,&y_grad);
    *interp_value = find_correct_eno_interpolation(enoimage,boundary_point); 
    return;
  }

  k=0;
  for (i=0;i<2*(NU_ENO_MAXORDER+1);i++){
    if ( interp_point[i].x != INVALID_POINT &&
     interp_point[i].y != INVALID_POINT ){
      tmp[k].x=interp_point[i].x;
      tmp[k].y=interp_point[i].y;
      k++;
    }
  }
  if (k == 0){
    /* printf("Enough interpolation points could not be found k=%d\n",k); */
    new_pt.x=(double)floor(boundary_point.x);
    new_pt.y=(double)floor(boundary_point.y);
    *first_der = find_grid_gradient(new_pt,enoimage,&x_grad,&y_grad);
    *interp_value = find_correct_eno_interpolation(enoimage,boundary_point); 
    return;
  }
  
  /* Fill the NU_ENO structure, with distance from the start point
     as x-axis and the ENO interpolated intensity value as the 
     y-axis */
  boundary_loc=euclidean_distance(tmp[0],boundary_point);
  x[0]=0.0;
  y[0]=(double)find_correct_eno_interpolation(enoimage,tmp[0]);
  for (i=1;i<k;i++){
    x[i]=(double)euclidean_distance(tmp[0],tmp[i]);
    if (boundary_loc >= x[i-1] && boundary_loc < x[i])
      boundary_idx=i-1;
    y[i]=(double)find_correct_eno_interpolation(enoimage,tmp[i]);
  }
  
  
  initialize_nu_eno_intervals(ei,2*(NU_ENO_MAXORDER+1));
  fill_eno_interval_info_general(ei,x,y,2*(NU_ENO_MAXORDER+1));
  find_nu_eno_interp_coeff(ei,2*(NU_ENO_MAXORDER+1));
  add_nu_eno_shock_interp_coeff(ei,2*(NU_ENO_MAXORDER+1));

  /*for (i=0;i<2*(NU_ENO_MAXORDER+1);i++)
    print_just_second_eno_coefficients(&ei[i]);

  init_draw_ps_original("interp.ps",25.0,15,15);
  draw_gridlines(0,0,15,15,9,data);
  point_draw_ps(grid_point.x,grid_point.y,BLUE,data);
  point_draw_ps(boundary_point.x,boundary_point.y,BLUE,data);
  for (i=0;i<2*(NU_ENO_MAXORDER+1);i++)
    point_draw_ps(tmp[i].x,tmp[i].y,RED,data);
  close_draw_ps();*/
  
  /* return nu_eno_interpolation(&ei[boundary_idx],boundary_loc); */
  *interp_value = nu_eno_interpolation(&ei[boundary_idx],boundary_loc);
  *first_der = nu_eno_interp_1derivative(&ei[boundary_idx],boundary_loc);
  *second_der = nu_eno_interp_2derivative(&ei[boundary_idx],boundary_loc);
  return;
  
}

    
/**************************************************************************
  Description: Finds enough grid line intersection points along the line 
               joining the grid_point and the boundary_point.
  Inputs : grid_point - The second point that specifies the line along
            which interpolation is done.
           boundary_point - Point around which interpolation is centered.
       N - No. of point needed,
       height,width - image dimensions.
  Output : interp_point - list of points found.
  **************************************************************************/

int compute_points_for_interpolation(Point grid_point, Point boundary_point,
                     Point *interp_point,int N, int height,
                     int width)
{
  
  int xdir,ydir;
  int i,j;
  int verbose=0;
  double slope, ct;
  Point loc,tmp;
  void *data;


  /*point_draw_ps(grid_point.x,grid_point.y,RED,data);
  point_draw_ps(boundary_point.x,boundary_point.y,BLUE,data);
  line_draw_ps(grid_point.x,grid_point.y,
           boundary_point.x,boundary_point.y,
           LIGHT_BLUE,data);*/
  
  for (i=0;i<2*N;i++){
    interp_point[i].x = INVALID_POINT;
    interp_point[i].y = INVALID_POINT;
  }
  

  find_tangent_line_directions(grid_point, boundary_point,&xdir,&ydir);
  /* printf("Dir=(%d,%d)\n",xdir,ydir); */

  find_line_equation(grid_point, boundary_point, &slope, &ct);
  /* printf("Slope,Const=(%f,%f)\n",slope, ct); */

  if (grid_point.x == 21 && grid_point.y == 5)
    i=1;
  

  tmp.x = boundary_point.x;
  tmp.y = boundary_point.y;
  i=N;
  while(i>0){
    find_next_grid_intersection(tmp,-xdir,-ydir,slope,ct,&loc);
    if (loc.x > width-1 || loc.x < 0 || loc.y > height-1 || loc.y < 0){
      break;
    }
    else{
      /* printf("%f T=(%f,%f) L=(%f,%f)\n",euclidean_distance(tmp,loc), */
      /*          tmp.x,tmp.y,loc.x,loc.y); */
      if (i==N || euclidean_distance(tmp,loc) > 0.1){
    /* point_draw_ps(loc.x,loc.y,GREY,data); */
    interp_point[i-1].x=loc.x;
    interp_point[i-1].y=loc.y;
    i--;
      }
      tmp.x=loc.x;
      tmp.y=loc.y; 
    }
  }
  if (i==N) /*No points allocated on one side -> interpolation cannot be done*/
    return 0; 

  tmp.x = boundary_point.x;
  tmp.y = boundary_point.y;
  i=N;
  while(i>0){
    find_next_grid_intersection(tmp,xdir,ydir,slope,ct,&loc);
    if (loc.x > width-1 || loc.x < 0 || loc.y > height-1 || loc.y < 0){
      break;
    }
    else{
      if (i==N || euclidean_distance(tmp,loc) > 0.1){
    /* point_draw_ps(loc.x,loc.y,GREY,data); */
    interp_point[2*N-i].x=loc.x;
    interp_point[2*N-i].y=loc.y;
    i--;
      }
      if (verbose){
    printf("Loc=(%f,%f)\n",loc.x,loc.y);
      }
      tmp.x=loc.x;
      tmp.y=loc.y;
    }
  }
  if (i==N) /*No points allocated on one side -> interpolation cannot be done*/
    return 0; 


  return 1;

}/*End of function compute_subpixel_force */
  

#ifdef __cplusplus
}
#endif





