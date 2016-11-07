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
  Description: Function that finds the snake force at a subpixel point 
               using ENO interpolation. ENO is done along a line 
           drawn from the grid point to the boundary point.
           
  Inputs :  grid point - Grid location
            boundary point - The closest point on the boundary.
        enoimage - ENO interpolated image (along normal grid lines)
        enosurface - ENO interpolated surface (along normal grid lines)
   Outputs : snake_force - the value of the snake force at the boundary

**************************************************************************/
void compute_subpixel_snake_force(Point grid_point, Point boundary_point,
                   ENO_Interval_Image_t *enoimage, 
                   ENO_Interval_Image_t *enosurface, double *snake_force)
{
  
  Point interp_point[2*(NU_ENO_MAXORDER+1)],tmp[2*(NU_ENO_MAXORDER+1)];
  Point new_pt;
  NU_ENO_Interval ei[2*(NU_ENO_MAXORDER+1)];
  void *data;
  double x[2*(NU_ENO_MAXORDER+1)],y[2*(NU_ENO_MAXORDER+1)];
  double boundary_loc,force;
  double gradI, gradSurf, gradStop;
  int interp_flag,i,pos,ix,iy,k;
  int height,width,boundary_idx;
  int verbose = 0;
  
  height=enoimage->height;
  width=enoimage->width;

  /*If the boundary point and the grid point coincide, then 
    simply use the central difference gradients to compute snake force */
  if (almost_equal(grid_point.x,boundary_point.x) &&
      almost_equal(grid_point.y,boundary_point.y) ){
    *snake_force = find_grid_snake_force_old(boundary_point,enoimage,enosurface);
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
    *snake_force = find_grid_snake_force_old(boundary_point,enoimage,enosurface);
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
    *snake_force = find_grid_snake_force_old(boundary_point,enoimage,enosurface);
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

  for (i=0;i<k-1;i++){
    gradI=(double)nu_eno_interp_1derivative(&ei[i],x[i]);
    y[i] = compute_edge_force(gradI);
  }
  gradI=(double)nu_eno_interp_1derivative(&ei[k-2],x[k-1]);
  y[k-1] = compute_edge_force(gradI);

  initialize_nu_eno_intervals(ei,2*(NU_ENO_MAXORDER+1));
  fill_eno_interval_info_general(ei,x,y,2*(NU_ENO_MAXORDER+1));
  find_nu_eno_interp_coeff(ei,2*(NU_ENO_MAXORDER+1));
  add_nu_eno_shock_interp_coeff(ei,2*(NU_ENO_MAXORDER+1));
  
  gradStop=nu_eno_interp_1derivative(&ei[boundary_idx],boundary_loc);
  
  x[0]=0.0;
  y[0]=(double)find_correct_eno_interpolation(enosurface,tmp[0]);
  for (i=1;i<k;i++){
    x[i]=(double)euclidean_distance(tmp[0],tmp[i]);
    if (boundary_loc >= x[i-1] && boundary_loc < x[i])
      boundary_idx=i-1;
    y[i]=(double)find_correct_eno_interpolation(enosurface,tmp[i]);
  }
  initialize_nu_eno_intervals(ei,2*(NU_ENO_MAXORDER+1));
  fill_eno_interval_info_general(ei,x,y,2*(NU_ENO_MAXORDER+1));
  find_nu_eno_interp_coeff(ei,2*(NU_ENO_MAXORDER+1));
  add_nu_eno_shock_interp_coeff(ei,2*(NU_ENO_MAXORDER+1));
  
  gradSurf=nu_eno_interp_1derivative(&ei[boundary_idx],boundary_loc);
  *snake_force = gradSurf*gradStop;
}

/**************************************************************************
  Description: Function that finds the snake force at a grid point. It uses
               central differences for finding the derivatives.
  Inputs :  boundary point - The point on the boundary.
        enoimage - ENO interpolated image (along normal grid lines)
  Return :  gradient at the boundary point        
**************************************************************************/
double find_grid_snake_force_old(Point boundary_point,ENO_Interval_Image_t *enoimage,
                ENO_Interval_Image_t *enosurface)
{

  double xgradI,ygradI;
  double xgradStop,ygradStop;
  double xgradSurf,ygradSurf;
  double gradSurf,gradI,gradStop;
  double force;

  gradI = find_grid_gradient(boundary_point,enoimage, &xgradI,&ygradI);
  xgradStop = compute_edge_force(xgradI);
  ygradStop = compute_edge_force(ygradI);

  gradSurf = find_grid_gradient(boundary_point,enosurface, &xgradSurf,&ygradSurf);
  
  return (xgradStop*xgradSurf + ygradStop*ygradSurf);
}

/**************************************************************************
  Description: Function that finds the snake force at a grid point. It uses
               ENO for finding the derivatives of the surface.
  Inputs :  x,y - The location in the image.
            Sx,Sy - the gradient of the stop function in the x & y directions.
            height,width - image dimensions
        enosurface - ENO interpolated surface (along normal grid lines)
  Return :  snake force at the grid point        
**************************************************************************/
double find_grid_snake_force(double *Sx,double *Sy,ENO_Interval_Image_t *enosurface, int x, int y,
                int height, int width)
{

  double xgradStop,ygradStop;
  double xgradSurf,ygradSurf;
  double gradSurf,gradI,gradStop;
  double snake_force;
  double Dxm,Dxp,Dym,Dyp,Dx,Dy;
  double Dx_upwind,Dy_upwind;
  int pos;

  xgradStop = Sx[y*width+x];
  ygradStop = Sy[y*width+x];

  Dxm = (double)eno_1derivative(&enosurface->horiz[y*width+(x-1)],
                   (double)enosurface->horiz[y*width+(x-1)].interp.end);
  Dxp = (double)eno_1derivative(&enosurface->horiz[y*width+(x)],
                   (double)enosurface->horiz[y*width+(x)].interp.start);
  Dym = (double)eno_1derivative(&enosurface->vert[(y-1)*width+x],
                   (double)enosurface->vert[(y-1)*width+x].interp.end);
  Dyp = (double)eno_1derivative(&enosurface->vert[y*width+x],
                   (double)enosurface->vert[y*width+x].interp.start);
  Dx = (Dxm+Dxp)/2;
  Dy = (Dym+Dyp)/2;
  snake_force = Dx*xgradStop + Dy*ygradStop;
  if (snake_force > 0.0){
    Dx_upwind = upwind_derivatives(Dxm,Dxp);
    Dy_upwind = upwind_derivatives(Dym,Dyp);
    return (Dx_upwind*xgradStop + Dy_upwind*ygradStop);
  }
  else{
    Dx_upwind = upwind_derivatives(-Dxm,-Dxp);
    Dy_upwind = upwind_derivatives(-Dym,-Dyp);
    return -(Dx_upwind*xgradStop + Dy_upwind*ygradStop);
  }
  
}
/**************************************************************************
  Description: Function that finds the snake force at a grid point. It uses
               central differences for finding the derivatives.
  Inputs :  boundary point - The point on the boundary.
        enoimage - ENO interpolated image (along normal grid lines)
  Return :  gradient at the boundary point        
**************************************************************************/
double find_grid_snake_force_cent_diff(Point boundary_point,double *x_grad,double *y_grad,
                      ENO_Interval_Image_t *enosurface)
{

  double stopxp1,stopxm1,stopyp1,stopym1;
  double xgradStop,ygradStop;
  double xgradSurf,ygradSurf;
  double gradSurf,gradI,gradStop;
  double snake_force;
  double Dxm,Dxp,Dym,Dyp,Dx,Dy;
  double Dx_upwind,Dy_upwind;
  int x,y,pos;
  int width,height;
  height = enosurface->height;
  width = enosurface->width;


  x=(int)boundary_point.x;
  y=(int)boundary_point.y;
  stopxp1 = compute_edge_force(x_grad[y*width+x+1]);
  stopxm1 = compute_edge_force(x_grad[y*width+x-1]);
  stopyp1 = compute_edge_force(y_grad[(y+1)*width+x]);
  stopym1 = compute_edge_force(y_grad[(y-1)*width+x]);
  xgradStop = (stopxp1 - stopxm1)/2.0;
  ygradStop = (stopyp1 - stopym1)/2.0;

  Dxm = (double)eno_1derivative(&enosurface->horiz[y*width+(x-1)],
                   (double)enosurface->horiz[y*width+(x-1)].interp.end);
  Dxp = (double)eno_1derivative(&enosurface->horiz[y*width+(x)],
                   (double)enosurface->horiz[y*width+(x)].interp.start);
  Dym = (double)eno_1derivative(&enosurface->vert[(y-1)*width+x],
                   (double)enosurface->vert[(y-1)*width+x].interp.end);
  Dyp = (double)eno_1derivative(&enosurface->vert[y*width+x],
                   (double)enosurface->vert[y*width+x].interp.start);
  Dx = (Dxm+Dxp)/2;
  Dy = (Dym+Dyp)/2;
  snake_force = Dx*xgradStop + Dy*ygradStop;
  if (snake_force > 0.0){
    Dx_upwind = upwind_derivatives(Dxm,Dxp);
    Dy_upwind = upwind_derivatives(Dym,Dyp);
    return (Dx_upwind*xgradStop + Dy_upwind*ygradStop);
  }
  else{
    Dx_upwind = upwind_derivatives(-Dxm,-Dxp);
    Dy_upwind = upwind_derivatives(-Dym,-Dyp);
    return -(Dx_upwind*xgradStop + Dy_upwind*ygradStop);
  }
  

  /*gradSurf = find_grid_eno_gradient(boundary_point,enosurface, &xgradSurf,&ygradSurf);*/
  /* return (xgradStop*xgradSurf + ygradStop*ygradSurf); */
}
/**************************************************************************
  Description: Function that finds the derivatives of the stop function
               at a grid point. It uses central differences for finding the derivatives.
  Inputs :  x_grad,y_grad - The image gradients in the x&y directions.
            x,y - location in the image.
  Outputs :  Sx,Sy - the gradient of the stop function in the x & y directions.
**************************************************************************/
double find_grid_stop_grad_cent_diff(double *x_grad,double *y_grad,int x,int y, 
                    int height,int width, double *Sx, double *Sy)
{

  double stopxp1,stopxm1,stopyp1,stopym1;
  double xgradStop,ygradStop;
  double xgradSurf,ygradSurf;

  if (x>0 && x<width-1 && y>0 && y<height-1){
    stopxp1 = compute_edge_force(x_grad[y*width+x+1]);
    stopxm1 = compute_edge_force(x_grad[y*width+x-1]);
    stopyp1 = compute_edge_force(y_grad[(y+1)*width+x]);
    stopym1 = compute_edge_force(y_grad[(y-1)*width+x]);
    xgradStop = (stopxp1 - stopxm1)/2.0;
    ygradStop = (stopyp1 - stopym1)/2.0;
    
    Sx[y*width+x] = xgradStop;
    Sy[y*width+x] = ygradStop;
  }
  else{
    Sx[y*width+x] = 0.0;
    Sy[y*width+x] = 0.0;
  }

}

/******************************************************************************
  Description : Find upwind derivatives.
  Inputs : u_l - left derivative
           u_r - right derivative
  Return : Upwind derivative
******************************************************************************/
double upwind_derivatives(double u_l, double u_r)
{
  double Dx;
  double min_ul_0,max_ur_0;
 
  if (u_l < 0)  min_ul_0 = u_l;
  else          min_ul_0 = 0.0;
  
  if (u_r > 0)  max_ur_0 = u_r;
  else          max_ur_0 = 0.0;
  
  return (min_ul_0 + max_ur_0)/2.0;
}

/**************************************************************************
  Description: Function that finds the snake force at a grid point. It uses
               central differences for finding the derivatives. Uses 
           (|phi_x,|phi_y).(I_x*I_xx + I_y*I_yx,I_x*I_xy + I_y*I_yy)
           as the snake force.
  Inputs :  boundary point - The point on the boundary.
        enoimage - ENO interpolated image (along normal grid lines)
  Return :  gradient at the boundary point        
**************************************************************************/
double find_grid_snake_force_hessian(double *Ixx_a, double *Iyy_a, double *Ixy_a, 
                    double *Iyx_a, double *Iy_a, double *Ix_a, 
                    ENO_Interval_Image_t *enosurface,
                    Point boundary_point)
{

  double Ixx,Iyy,Ixy,Iyx,Ix,Iy;
  double x_snake,y_snake;
  double snake_force;
  double Dxm,Dxp,Dym,Dyp,Dx,Dy;
  double Dx_upwind,Dy_upwind;
  int x,y,pos;
  int width,height;
  height = enosurface->height;
  width = enosurface->width;


  x=(int)boundary_point.x;
  y=(int)boundary_point.y;
  Ixx = Ixx_a[y*width+x];
  Iyy = Iyy_a[y*width+x];
  Ixy = Ixy_a[y*width+x];
  Iyx = Iyx_a[y*width+x];
  Ix = Ix_a[y*width+x];
  Iy = Iy_a[y*width+x];
  x_snake = Ix*Ixx + Iy*Iyx;
  y_snake = Ix*Ixy + Iy*Iyy;

  Dxm = (double)eno_1derivative(&enosurface->horiz[y*width+(x-1)],
                   (double)enosurface->horiz[y*width+(x-1)].interp.end);
  Dxp = (double)eno_1derivative(&enosurface->horiz[y*width+(x)],
                   (double)enosurface->horiz[y*width+(x)].interp.start);
  Dym = (double)eno_1derivative(&enosurface->vert[(y-1)*width+x],
                   (double)enosurface->vert[(y-1)*width+x].interp.end);
  Dyp = (double)eno_1derivative(&enosurface->vert[y*width+x],
                   (double)enosurface->vert[y*width+x].interp.start);
  Dx = (Dxm+Dxp)/2;
  Dy = (Dym+Dyp)/2;
  snake_force = Dx*x_snake + Dy*y_snake;
  if (snake_force > 0.0){
    Dx_upwind = upwind_derivatives(Dxm,Dxp);
    Dy_upwind = upwind_derivatives(Dym,Dyp);
    return (Dx_upwind*x_snake + Dy_upwind*y_snake);
  }
  else{
    Dx_upwind = upwind_derivatives(-Dxm,-Dxp);
    Dy_upwind = upwind_derivatives(-Dym,-Dyp);
    return -(Dx_upwind*x_snake + Dy_upwind*y_snake);
  }
  

  /*gradSurf = find_grid_eno_gradient(boundary_point,enosurface, &xgradSurf,&ygradSurf);*/
  /* return (xgradStop*xgradSurf + ygradStop*ygradSurf); */
}

#ifdef __cplusplus
}
#endif





