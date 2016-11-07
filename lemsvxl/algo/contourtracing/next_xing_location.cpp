#include <stdio.h>
#include <vcl_cmath.h>
#include "graphics.h"


#define EPSILON 1E-2

int next_xing_debug=0;

int next_xing_location(CurrPt *curr_pt,CurrPt *next_pt,CurrPt *end_pt,XingsLoc  *vert_xings, 
                        XingsLoc *horiz_xings, int sss, double label, int height, int width)
     
{
   
  int xx,yy,flag,s_dir,good_crossing,xr,yr;
  int position;
  double x,y,rem_x,rem_y,rem_xr,rem_yr;
  
  
  x = curr_pt->x;
  y = curr_pt->y;
  
  xx = (int) x;
  yy = (int) y;
  
  rem_x = fabs(x-xx);
  rem_y = fabs(y-yy);
  
  xr= (int)(x+0.5);
  yr= (int)(y+0.5);
  
  rem_xr = fabs(x-xr);
  rem_yr = fabs(y-yr);
  
  if (rem_xr < EPSILON && rem_yr < EPSILON) {
    xx = xr;
    yy = yr;
    rem_x = rem_xr;
    rem_y = rem_yr;
  }
  
  s_dir = sss;
  
  next_pt->x = NOXING;   
  next_pt->y = NOXING;
  
   /*    
         - 22   11     03 
         
         23 * 21  *  10  * 04
         
         -  30     20     02 
         
         31 * 40  P  00  * 01
         
         -  41    60     70   
          

         42 * 50  *  61  * 62

         - 43    51    63
          
          */

  /*Grid zero crossing */
   if ((rem_y < EPSILON) && (rem_x < EPSILON)) {
     /////if (next_xing_debug) 
     /////  printf("s_dir =%i and grid crossing\n", s_dir); 
     switch (s_dir) {
     case (0): {
       /* 00  inner leg */
        good_crossing = one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,  
                                       xx,yy,1, 1, height,width); 
       /* 01  middle right corner*/
       good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1), yy,(xx+1), yy,1,1,height,width);
       /*  02  right up vert side */
       if (!good_crossing) 
         good_crossing = one_d_crossing(vert_xings, next_pt,curr_pt,end_pt, label,   
                                        (xx+1),(yy-1), 2, 2, height,width);
       /* 03    */
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1), (yy-2), (xx+1), (yy-1),2,2,height,width);
       /* 04    */
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1), (yy-1), (xx+1), (yy-1),1,1,height,width);
       break;
     }
     case (1): {  
       /* 10 */
       good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                     xx,(yy-1),2,1, height,width);
       /* 11 */
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         xx, (yy-2), xx, (yy-1),2,2,height,width);
       break;
     }
     case (2): {
       /* 20 */
       good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                     xx,(yy-1),1,2, height,width);
      /* 21 */
       if (!good_crossing) 
         good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                       (xx-1),(yy-1),2,1, height,width);
       /* 22 */
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx-1), (yy-2), (xx-1), (yy-1),2,2,height,width);
       /* 23 */
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx-2), (yy-1), (xx-1), (yy-1),2,1,height,width);
       break;
     }
     case (3): {
       /* 30 */
      good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                    (xx-1),(yy-1),1,2, height,width);
     /* 31 */
      if (!good_crossing) 
        good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                        (xx-2), yy, (xx-1), yy,2,1,height,width);
      break;
     }
     case (4): {
       /* 40 */
       good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                     (xx-1),yy,1,1, height,width);
       /* 41 */ 
       if (!good_crossing) 
         good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                       (xx-1),yy,1,2, height,width);

       /* 42 */ 
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx-2), (yy+1),(xx-1), (yy+1),2,1,height,width);
       /* 43 */ 
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx-1), (yy+1), (xx-1), (yy+1),1,2,height,width);
       break;
     }
     case (5): {
       /* 50 */
       good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                     (xx-1), (yy+1),1,1, height,width);
       /* 5-6 */ 
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,
                                         xx, (yy+1), xx, (yy+1),1,1,height,width);
       /* 51 */ 
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         xx, (yy+1), xx, (yy+1),1,2,height,width);
       break;
     }
     case (6): {
         /* 60 */
       good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                     xx,yy,2,2, height,width); 
     /* 61 */
       good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                       xx,(yy+1),1,1, height,width);
       /* 62 */ 

       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1), (yy+1), (xx+1), (yy+1),1,1,height,width);
       
       /* 63 */ 
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1), (yy+1), (xx+1), (yy+1),1,2,height,width);
       break;
     }
     case (7): {
       good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                     (xx+1),yy,2,2, height,width);
       break;
     }
     }
   }

   /*    zero along a VERTICAL grid line    */
   else if (rem_x == 0) {  
     /////if (next_xing_debug) 
     /////  printf("s_dir =%i and vertical crossing\n", s_dir);
     switch (s_dir) {
     case (0): {
       if (rem_y > 0.5){
   good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
               (xx+1),yy,2,2, height,width);
   if (!good_crossing){
     good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
           (xx+1),yy,1,2, height,width);
   }
       }
       else{
   good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
               (xx+1),yy,1,2, height,width);
   if (!good_crossing){
     good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
           (xx+1),yy,2,2, height,width);
   }
       }
   
       /* top right corner (vertical) */
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1), (yy-1), (xx+1), yy,2,2,height,width);
       if (!good_crossing) /*Added 01-31-98*/
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1), yy, (xx+1), yy,2,2,height,width);
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1), yy, (xx+1), yy,1,1,height,width);

       break;
     }
     case (1): {
       good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                       xx,yy,1,1, height,width);
       if (!good_crossing) 
   good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                       xx,yy,2,1, height,width);

       if (!good_crossing) 
   good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
           xx, (yy-1), xx, yy,2,1,height,width);
       if (!good_crossing) 
   good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
           (xx+1), (yy-1), xx, yy,2,2,height,width);
       break;
     }
     case (2): {
       if (vert_xings[yy*width+xx].cnt) {
         if (vert_xings[yy*width+xx].loc[0] < y) {
           good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         xx,yy,1,2, height,width);
         }
       }
       break;
     }
     case (3): {
       good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                      (xx-1),yy,2,1, height,width);
       if (!good_crossing)
   good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
               (xx-1),yy,1,1, height,width);
       /* top left corner */
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx-1), (yy-1), (xx-1), yy,2,2,height,width);
       if (!good_crossing)  /*Added 01-31-98*/
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx-1), yy, (xx-1), yy,2,2,height,width);
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx-2), yy, (xx-1), yy,2,1,height,width);

       break;
     }
     case (4): {
       if (rem_y > 0.5){
   good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
               (xx-1),yy,2,2, height,width);
   if (!good_crossing){
     good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
           (xx-1),yy,1,2, height,width);
   }
       }
       else{
   good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
               (xx-1),yy,1,2, height,width);
   if (!good_crossing){
     good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
           (xx-1),yy,2,2, height,width);
   }
       }
       
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx-2), (yy+1), (xx-1),(yy+1),2,1,height,width);
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx-1), (yy+1), (xx-1),(yy+1),1,2,height,width);

       break;
     } 
     case (5): {
       good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                      (xx-1),(yy+1),2,1, height,width);
       if (!good_crossing) 
   good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
               (xx-1),(yy+1),1,1, height,width);
       /* bottom corner */
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         xx, (yy+1), xx,(yy+1),1,2,height,width);
       break;
     }
     case (6): {
       if (vert_xings[yy*width+xx].cnt) {
         if (vert_xings[yy*width+xx].cnt == 2) {
           if (vert_xings[yy*width+xx].loc[1] > y) {
             good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                           xx,yy,2,2, height,width);
           }
         }
         else if (vert_xings[yy*width+xx].loc[0] > y) {
            good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                           xx,yy,1,2, height,width);
         }
       }
       break;
     } 
     case (7): {
       good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                     xx,(yy+1),1,1, height,width);
       if (!good_crossing)
   good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
               xx,(yy+1),2,1, height,width);
       /* bottom right corner */
       if (!good_crossing)
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1), (yy+1), (xx+1),(yy+1),1,2,height,width);
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1), (yy+1), (xx+1),(yy+1),1,1,height,width);

       break;
     }
     }
   }  

/* zero along a horizontal grid line */ 

   else if (rem_y == 0) {
     if (next_xing_debug)
       printf("s_dir =%i and horizontal crossing\n", s_dir);
     switch (s_dir) {
     case (0): {
       if (horiz_xings[yy*width+xx].cnt ==2) {
         if (horiz_xings[yy*width+xx].loc[1] > x) {
           good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         xx,yy,2,1, height,width);
         }
       }
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1), yy, (xx+1),yy,1,1,height,width);

       break;
     }
     case (1): {
       good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                     (xx+1),(yy-1),2,2, height,width);
       if (!good_crossing) 
   good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
               (xx+1),(yy-1),1,2, height,width);
       /* top right corner */
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx), (yy-1), (xx+1),(yy-1),2,1,height,width);
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1), (yy-2), (xx+1),(yy-1),2,2,height,width);
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1), (yy-1), (xx+1),(yy-1),1,1,height,width);
       break;
     }
     case (2): {
       if (rem_x >0.5){
   good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
               xx,(yy-1),2,1, height,width);
   if (!good_crossing){
     good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
           xx,(yy-1),1,1, height,width);
   }
       }
       else{
   good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
               xx,(yy-1),1,1, height,width);
   if (!good_crossing){
     good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
           xx,(yy-1),2,1, height,width);
   }
       }
       /* top left */
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         xx, (yy-2), xx,(yy-1),2,2,height,width);
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx-1), (yy-1), xx,(yy-1),2,1,height,width);
       break;
     }
     case (3): {
       good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
               xx,(yy-1),2,2, height,width);
       if (!good_crossing) 
   good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
               xx,(yy-1),1,2, height,width);
       /* center left */
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx-1), yy, xx,yy,2,1,height,width);
       break;
     }
     case (4): {
       if (horiz_xings[yy*width+xx].cnt) {
         if (horiz_xings[yy*width+xx].loc[0] < x) {
           good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,  
                                         xx,yy,1,1, height,width);
         }
       }
       break;
     } 
     case (5): {
       good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                     xx,yy,1,2, height,width);
       if (!good_crossing)
   good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,
               xx,yy,2,2, height,width);
       /* bottom left */
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label, 
                                         (xx-1),(yy+1),xx,(yy+1),2,1,height,width);
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         xx, (yy+1), xx,(yy+1),1,2,height,width);
       break;
     }
     case (6): {
       if (rem_x >0.5){
   good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
               xx,(yy+1),2,1, height,width);
   if (!good_crossing){
     good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
           xx,(yy+1),1,1, height,width);
   }
       }
       else{
   good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
               xx,(yy+1),1,1, height,width);
   if (!good_crossing){
     good_crossing= one_d_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,   
           xx,(yy+1),2,1, height,width);
   }
       }
       /* bottom right */
       if (!good_crossing) 
         good_crossing = corner_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                         (xx+1),(yy+1),(xx+1),(yy+1),1,2,height,width);
       if (!good_crossing) 
         good_crossing = corner_crossing(horiz_xings, next_pt, curr_pt,end_pt, label,  
                                         (xx+1),(yy+1),(xx+1),(yy+1),1,1,height,width);
       break;
     }
     case (7): {
       good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,   
                                     (xx+1),yy,1,2, height,width);
       if (!good_crossing)
   good_crossing= one_d_crossing(vert_xings, next_pt, curr_pt,end_pt, label,
               (xx+1),yy,2,2, height,width);
       break;
     }
     }
   }

#ifdef TEST
   if (next_pt->x >=0.0) {
     yy = (int) next_pt->y;
     xx = (int) next_pt->x;
     
     rem_y = next_pt->y-(int) yy;
     rem_x = next_pt->x-(int) xx;
     
     if ((fabs(rem_y) < EPSILON) && (fabs(rem_x) < EPSILON)) {
       position=yy*width+xx;
       vert_xings[position].loc[0] = -label;
       vert_xings[position].loc[1] = -label;
       horiz_xings[position].loc[0] = -label;
       horiz_xings[position].loc[1] = -label;
     }
   }
   position=yy*width+xx;
   if (vert_xings[position].loc[0] < 0.0) {
     if (horiz_xings[position].loc[0] ==  next_pt->x)
       horiz_xings[position].loc[0] = -label;
   }
   if (horiz_xings[position].loc[0] < 0.0) {
     if (vert_xings[position].loc[0] ==  next_pt->y)
       vert_xings[position].loc[0] = -label;
   }
#endif 

  //!!!
  return 0;
}


int one_d_crossing(XingsLoc *xings, CurrPt *next_pt, CurrPt *curr_pt,  CurrPt *end_pt,
                   double label, int x, int y, int order, int direction, int height, int width)

{
  int flag,xx,yy;  
  double crossing,rem_x,rem_y;

  if (next_xing_debug) 
    if (direction == 2) 
      printf("trying to find a vertical xing at  (%i,%i)\n", y,x);
    else 
      printf("trying to find a horizontal xing at (%i,%i)\n", y,x);

  flag =0;
  
  if (y < 0 || y > (height-1) || x< 0 || x >(width-1)) {
    flag=0;
    next_pt->y = BOUNDARY;
    next_pt->x = BOUNDARY;
    end_pt->type = (int) BOUNDARY;
    end_pt->y = (double) y;
    end_pt->x = (double) x;
    return 0;
  }

  if (xings[y*width+x].cnt) {
    if ((order == 2) && (xings[y*width+x].loc[1] < 0.0)) {
      next_pt->y = xings[y*width+x].loc[1];
      next_pt->x = xings[y*width+x].loc[1];
      flag =0;
    }  
    else if ((order ==2) && (xings[y*width+x].cnt == 2)) {
      crossing = xings[y*width+x].loc[1];
      if (direction == 1) {
        if ((curr_pt->x == crossing && curr_pt->y == y) ||
      ((label == UNLABELLED) || (xings[y*width+x].label[1] != label) &&
       (xings[y*width+x].label[1] != UNLABELLED)) ){
          flag = 0;
        }
        else {
          next_pt->x = crossing;
          next_pt->y = (double) y;
          xings[y*width+x].loc[1] = UNLABELLED;
          flag = 1;
        }
      }
      else if (direction == 2){
        if ( (curr_pt->y == crossing && curr_pt->x == x) ||
       ((label == UNLABELLED) || (xings[y*width+x].label[1] != label) &&
        (xings[y*width+x].label[1] != UNLABELLED)) )  {
          flag = 0;
        }
        else {
          next_pt->y = crossing;
          next_pt->x = (double) x;
           xings[y*width+x].loc[1] = UNLABELLED;
           flag = 1;
        }
      }
    }  
    
    if (((flag == 0) || (order == 1)) && (xings[y*width+x].loc[0] < 0.0)) {
      if (curr_pt->x == x && curr_pt->y == y) {
        next_pt->y = xings[y*width+x].loc[0];
        next_pt->x = xings[y*width+x].loc[0];
        flag =0;
      }
    } 
    else if (((flag ==0) || (order ==1)) && (xings[y*width+x].cnt)) {
      crossing = xings[y*width+x].loc[0];
      if (direction == 1) {
        if ((curr_pt->x == crossing && curr_pt->y == y)  ||
      ((label == UNLABELLED) || (xings[y*width+x].label[0] != label) &&
       (xings[y*width+x].label[0] != UNLABELLED)) ){
          flag = 0;
        }
        else {
          next_pt->x = crossing;
          next_pt->y = (double) y;
          xings[y*width+x].loc[0] = UNLABELLED;
          /* xings[y*width+x].loc[1] = UNLABELLED; */
          flag = 1;
        }
      }
      else if (direction == 2) {
        if ((curr_pt->y == crossing && curr_pt->x == x)  ||
      ((label == UNLABELLED) || (xings[y*width+x].label[0] != label) &&
       (xings[y*width+x].label[0] != UNLABELLED)) ) {
          flag = 0;
        }
        else {
          next_pt->y = crossing;
          next_pt->x = (double) x;
          xings[y*width+x].loc[0] = UNLABELLED;
          /* xings[y*width+x].loc[1] = UNLABELLED; */
          flag = 1;
        }
      }
    }

  }
  
  if (next_pt->x != -label && end_pt->type == NOXING) {
    end_pt->type = (int) next_pt->x;
    end_pt->y = (double) y;
    end_pt->x = (double) x;
  }

  /*if (next_xing_debug) 
    if (flag != 0) {
      printf("loc: (%i,%i), and it found at %f and %f\n",y,x,next_pt->y,next_pt->x);
      printf("marked?: [0] = %f and [1] =%f\n", xings[y*width+x].loc[0],
             xings[y*width+x].loc[1]);
    }
    else {
      printf("no xing\n");
    }*/
  
  return(flag);
  
}
    
int corner_crossing(XingsLoc *xings, CurrPt *next_pt,CurrPt *curr_pt, CurrPt *end_pt,
                    double label,int x, int y, int corner_x, int corner_y, 
                    int order, int direction, int height, int width)
     
{
  int flag;
  double crossing;

 /*if (next_xing_debug) 
   if (direction == 2) 
     printf("corner: trying to find a vertical xing at  (%i,%i)\n", y,x);
   else 
     printf("corner: trying to find a horizontal xing at (%i,%i)\n", y,x);*/


 if (y < 0 || y > (height-1) || x< 0 || x> (width-1)) {
   next_pt->y = BOUNDARY;
   next_pt->x = BOUNDARY;
   end_pt->type = (int) BOUNDARY;
   end_pt->y = (double) y;
   end_pt->x = (double) x;
   return 0;
 }

  flag =0;
  if (xings[y*width+x].cnt) {
    if ((order == 2) && (xings[y*width+x].loc[1] <0.0)) {
      next_pt->y = xings[y*width+x].loc[1];
      next_pt->x = xings[y*width+x].loc[1];
      flag = 0;
    }  
    else if ((order ==2) && (xings[y*width+x].cnt == 2)) {
      crossing = xings[y*width+x].loc[1];
      if (direction == 1) {
        if ((curr_pt->x == crossing && curr_pt->y == y) ||
      xings[y*width+x].label[1] != UNLABELLED &&
      xings[y*width+x].label[1] != label){
          flag = 0;
          return(flag);
        }
        else if (fabs(crossing - corner_x) <= EPSILON) {
          next_pt->y = (double) corner_y;
          next_pt->x = (double) corner_x;
          /* next_pt->y = (double) y; */
          xings[y*width+x].loc[1] = UNLABELLED;
          /* xings[y*width+x].loc[0] = UNLABELLED; */
          flag = 1;
        }
      }
      else if (direction == 2){
        if ((curr_pt->y == crossing && curr_pt->x == x) ||
      xings[y*width+x].label[1] != UNLABELLED &&
      xings[y*width+x].label[1] != label) {
          flag = 0;
          return(flag);
        }
        else if (fabs(crossing - corner_y) <= EPSILON) {
          next_pt->y = (double) corner_y;
          next_pt->x = (double) corner_x;
          /* next_pt->x = (double) x; */
          xings[y*width+x].loc[1] = UNLABELLED;
          /* xings[y*width+x].loc[0] = UNLABELLED; */
          flag = 1;
        }
      }
    }   
      
    if (((flag == 0) || (order == 1)) && (xings[y*width+x].loc[0] < 0.0)) {
      if ((curr_pt->x == x && curr_pt->y == y) ||
    xings[y*width+x].label[0] != UNLABELLED &&
    xings[y*width+x].label[0] != label) {
        next_pt->y = xings[y*width+x].loc[0];
        next_pt->x = xings[y*width+x].loc[0];
        flag =0;
      }
    }
    else if (((flag ==0) || (order ==1)) && (xings[y*width+x].cnt)) {
      crossing = xings[y*width+x].loc[0];
      if (xings[y*width+x].label[0] != UNLABELLED &&
    xings[y*width+x].label[0] != label){
  flag = 0;
  return(flag);
      }
      if (direction == 1) {
        if (curr_pt->x == crossing && curr_pt->y == y) {
          flag = 0;
          return(flag);
        }
        else if (fabs(crossing - corner_x) <= EPSILON) {
          next_pt->y = (double) corner_y;
          next_pt->x = (double) corner_x;
          xings[y*width+x].loc[0] = UNLABELLED; 
          /* xings[y*width+x].loc[1] = UNLABELLED; */
          flag = 1;
        }
      }
      else if (direction == 2) {
        if (curr_pt->y == crossing && curr_pt->x == x) {
          flag = 0;
          return(flag);
        }
        else if (fabs(crossing - corner_y) <= EPSILON) {
          next_pt->y = (double) corner_y;
          next_pt->x = (double) corner_x;
          xings[y*width+x].loc[0] = UNLABELLED;
          /* xings[y*width+x].loc[1] = UNLABELLED; */
          flag = 1;
        }
      }
    }
  }
  
  if (next_pt->x != -label && next_pt->x != NOXING) {
    end_pt->type = (int) next_pt->y;
    end_pt->y = (double) corner_y;
    end_pt->x = (double) corner_x;
  }
  
  /*if (next_xing_debug) 
    if (flag != 0) {
      printf("loc: (%i,%i), and it found at %f and %f\n",y,x,next_pt->y,next_pt->x);
      printf("marked?: [0] = %f and [1] =%f\n", xings[y*width+x].loc[0],
             xings[y*width+x].loc[1]);
    }
    else {
      printf("no xing\n");
    }*/

  return(flag);

}

 
