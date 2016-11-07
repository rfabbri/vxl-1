#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dbseg_subpixel_postscript.h"
#include "dbseg_subpixel_1d_eno_types.h"
#include "dbseg_subpixel_non-uniform-eno.h"
#include "dbseg_subpixel_SPCedtGeno.h"

#include "dbseg_subpixel_subpixel_bubbles.h"


double find_correct_eno_interpolation(ENO_Interval_Image_t *enoimage, Point pt)
{
  int pt_type;
  int x,y,pos;
  int width,height;
  double interp_value;

  width = enoimage->width;
  height = enoimage->height;

  if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height){
    /* printf("Invalid Point Pt = (%f,%f)\n",pt.x,pt.y); */
    return IMPOSSIBLE_VALUE;
  }
  pt_type=xing_type(&pt);

  if (pt_type == CELL){
    printf("ENO Interpolation cannot be done for a CELL location.\n");
    printf("Point has to lie on a grid line  Pt = (%f,%f)\n",pt.x,pt.y);
    return IMPOSSIBLE_VALUE;
  }

  x=(int)floor(pt.x);
  y=(int)floor(pt.y);

  if (x == width-1)
    x--;
  if (y == height-1)
    y--;
  pos=y*width+x;
  if (pt_type == HORIZ)
    interp_value = eno_interpolation(&enoimage->horiz[pos],pt.x);
  else if (pt_type == VERT)
    interp_value = eno_interpolation(&enoimage->vert[pos],pt.y);
  else
    interp_value = eno_interpolation(&enoimage->horiz[pos],pt.x);
  return interp_value;
}
    
/**************************************************************************
  Description: Function that finds the gradient at a grid point. It uses
               ENO for finding the derivatives.
  Inputs :  boundary point - The point on the boundary.
        enoimage - ENO interpolated image (along normal grid lines)
  Outputs: grad_x,grad_y - gradients in the X & Y directions respectively
  Return :  gradient at the boundary point        

**************************************************************************/

double find_grid_eno_gradient(Point pt, ENO_Interval_Image_t *enoimage, 
                 double *grad_x, double *grad_y)
{
  
  int pt_type;
  int x,y,pos;
  int width,height;
  double grad;

  width = enoimage->width;
  height = enoimage->height;
  if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height){
    /* printf("Invalid Point Pt = (%f,%f)\n",pt.x,pt.y); */
    return IMPOSSIBLE_VALUE;
  }

  pt_type=xing_type(&pt);

  if (pt_type != GRID){
    printf("Not on a grid. In find_grid_eno_gradient.\n");
    printf("Point has to lie on a grid line  Pt = (%f,%f)\n",pt.x,pt.y);
    return IMPOSSIBLE_VALUE;
  }

  x=(int)floor(pt.x);
  y=(int)floor(pt.y);

  if (x == width-1)
    x--;
  if (y == height-1)
    y--;
  pos=y*width+x;
  
  *grad_x=eno_1derivative(&enoimage->horiz[pos],pt.x);
  *grad_y=eno_1derivative(&enoimage->vert[pos],pt.y);
  
  grad = sqrt(pow(*grad_x,2.0)+pow(*grad_y,2.0));
  
  return grad;
}

/**************************************************************************
  Description: Function that finds the gradient at a grid point. It uses
               ENO for finding the derivatives.
  Inputs :  boundary point - The point on the boundary.
        enoimage - ENO interpolated image (along normal grid lines)
  Outputs: grad_x,grad_y - gradients in the X & Y directions respectively
  Return :  gradient at the boundary point        

**************************************************************************/

double find_grid_eno_2nd_der(Point pt, ENO_Interval_Image_t *enoimage, 
                double *x_2der, double *y_2der)
{
  
  int pt_type;
  int x,y,pos;
  int width,height;
  double grad;

  width = enoimage->width;
  height = enoimage->height;
  if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height){
    /* printf("Invalid Point Pt = (%f,%f)\n",pt.x,pt.y); */
    return IMPOSSIBLE_VALUE;
  }

  pt_type=xing_type(&pt);

  if (pt_type != GRID){
    printf("Not on a grid. In find_grid_eno_gradient.\n");
    printf("Point has to lie on a grid line  Pt = (%f,%f)\n",pt.x,pt.y);
    return IMPOSSIBLE_VALUE;
  }

  x=(int)floor(pt.x);
  y=(int)floor(pt.y);

  if (x == width-1)
    x--;
  if (y == height-1)
    y--;
  pos=y*width+x;
  
  *x_2der=eno_2derivative(&enoimage->horiz[pos],pt.x);
  *y_2der=eno_2derivative(&enoimage->vert[pos],pt.y);
  
  grad = sqrt(pow(*x_2der,2.0)+pow(*y_2der,2.0));
  
  return grad;
}

#ifdef __cplusplus
}
#endif





