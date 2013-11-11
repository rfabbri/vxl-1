#include <stdio.h>
#include <stdlib.h>
#include <vcl_cmath.h>

#include "graphics.h"

#define DC_S 1

/********************************************************************
  Description : Given two points, predicts the direction where to look 
  for the next ZEROX. 
  Inputs : curr - current point
           prev - previous point
  Output : sign - +1 if next direction is in CCW direction
                  -1 if next direction is in CW direction
  Return : next direction
********************************************************************/

int find_initial_tracer_direction(Tr_Point curr, Tr_Point prev, int *sign)
{
  int xdir,ydir,tx,ty,pt_type,horiz_flag,new_dir;
  double slope,Diag_slope,ct,rem_x,rem_y;
  Tr_Point Diag;
  int vert_dir[2][3][3]={{{4,4,4},{2,-10,6},{0,0,0}},
       {{3,4,5},{2,-10,6},{1,0,7}}};

  int horiz_dir[2][3][3]={{{3,4,5},{2,-10,6},{1,0,7}},
       {{2,4,6},{2,-10,6},{2,0,6}}};

  int grid_dir[2][3][3]={{{3,4,4},{2,-10,6},{0,0,7}},
       {{2,4,5},{2,-10,6},{1,0,6}}};
  int vert_sign[2][3][3]={{{-1,DC_S,1},{DC_S,DC_S,DC_S},{1,DC_S,-1}},
        {{1,DC_S,-1},{DC_S,DC_S,DC_S},{-1,DC_S,1}}};

  int horiz_sign[2][3][3]={{{-1,DC_S,1},{DC_S,DC_S,DC_S},{1,DC_S,-1}},
         {{1,DC_S,-1},{DC_S,DC_S,DC_S},{-1,DC_S,1}}};

  tr_compute_directions(curr,prev,&xdir,&ydir);
  tr_find_line_equation(curr,prev,&slope,&ct);
  pt_type=tr_xing_type(&curr);
  rem_x = curr.x - floor(curr.x);
  rem_y = curr.y - floor(curr.y);
  /* printf("rem_x=%f,rem_y=%f\n",rem_x,rem_y); */

  /*printf("Cur=(%f,%f) Prev=(%f,%f) Pt_type = %d\n",
   curr.x,curr.y,prev.x,prev.y,pt_type);*/
  
  if (curr.x == 23.0 && curr.y > 23.0 && curr.y < 24.0)
    Diag.y=0;


  if (pt_type == TR_HORIZ){
    Diag.y = (double)floor(curr.y) + (double)ydir;
    if (xdir > 0) 
      Diag.x = (double)floor(curr.x) + 1.0;
    else
      Diag.x = (double)floor(curr.x);
  }
  else if (pt_type == TR_VERT){
    Diag.x = (double)floor(curr.x) + (double)xdir;
    if (ydir > 0) 
      Diag.y = (double)floor(curr.y) + 1.0;
    else
      Diag.y = (double)floor(curr.y);
  }
  else if (pt_type == TR_CELL){
    if (xdir < 0)
      tx=0;
    else
      tx=xdir;
    if (ydir < 0)
      ty=0;
    else
      ty=ydir;

    Diag.x = (double)floor(curr.x) + (double)tx;
    Diag.y = (double)floor(curr.y) + (double)ty;
  }
  else if (pt_type == TR_GRID){
    Diag.x = (double)floor(curr.x) + (double)xdir;
    Diag.y = (double)floor(curr.y) + (double)ydir;
  }

  Diag_slope=tr_find_slope(curr,Diag);    
  
  if (fabs(Diag_slope) >= fabs(slope) && 
      fabs(slope) < TR_VERY_LARGE_VALUE ) /* Look in the vertical interval*/
    horiz_flag = 0;
  else               /* Look in the horizontal interval*/
    horiz_flag=1;

     

  /* printf("Dsl=%d Xdir=%d ydir=%d\n",horiz_flag,xdir,ydir); */
  if (pt_type == TR_HORIZ){
    /* printf("Dir=%d\n",horiz_dir[horiz_flag][xdir+1][ydir+1]); */
    new_dir =  horiz_dir[horiz_flag][xdir+1][ydir+1];
  }
  else if (pt_type == TR_VERT){
    /* printf("Dir=%d\n",vert_dir[horiz_flag][xdir+1][ydir+1]); */
    new_dir =  vert_dir[horiz_flag][xdir+1][ydir+1];
  }
  else if (pt_type == TR_GRID){
    /* printf("Dir=%d\n",grid_dir[horiz_flag][xdir+1][ydir+1]); */
    new_dir =  grid_dir[horiz_flag][xdir+1][ydir+1];
  }

  /* printf("Dsl=%d Xdir=%d ydir=%d\n",horiz_flag,xdir,ydir); */
 /*Find the sign of next direction*/
  if (xdir != 0 && ydir != 0)
    *sign = horiz_sign[horiz_flag][xdir+1][ydir+1];
  else if (pt_type == TR_HORIZ){
    if (new_dir == 6)
      if (rem_x > 0.5)
  *sign = 1;
      else
  *sign = -1;
    else if (new_dir == 2)
      if (rem_x < 0.5)
  *sign = 1;
      else
  *sign = -1;
  }
  else if (pt_type == TR_VERT){
    if (new_dir == 4)
      if (rem_x > 0.5)
  *sign = 1;
      else
  *sign = -1;
    else if (new_dir == 0)
      if (rem_x < 0.5)
  *sign = 1;
      else
  *sign = -1;
  }
  return new_dir;
}
     
