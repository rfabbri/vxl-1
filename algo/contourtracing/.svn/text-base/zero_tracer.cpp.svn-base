#include <vcl_cmath.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


#include "graphics.h"
/***************************************************************************
  Description: Trace zero crossing samples 
  Inputs :  xings - zero samples created by user
            label - array of labels. Different labels represent different
      objects.
      height,width - image dimensions
  Output: Tr - The traced ENO Zerox.
***************************************************************************/


void  trace_zero_xings(Xings *xings, Tracer *Tr, int height, int width) 

{
  int y,x,pos,ii,jj,id;
  int contours_ptr=0;
  int no_of_contours,pts_ina_contour;
  double yd,xd,count;
  FILE *f1; //*fopen(), 

  /* allocate memory for tracer */ 
  contours_ptr = height*width;
  contours_ptr = height*width;

  allocate_data_tracer(Tr,contours_ptr);
  
  subpixel_contour_tracer_all(Tr, xings, height, width); 
  fix_tracer_labels(Tr);

  ii=0;
  no_of_contours=0; pts_ina_contour=0;
  while(Tr->vert[ii] != ENDOFLIST && Tr->horiz[ii] != ENDOFLIST){
    pts_ina_contour=0;
    no_of_contours++;
    while(Tr->vert[ii] != ENDOFCONTOUR && Tr->horiz[ii] != ENDOFCONTOUR){
      pts_ina_contour++;
      ii++;
    }
    /* printf("Found a contour of length %d\n",pts_ina_contour); */
    ii++;
  }
  printf("Found %d contour(s)\n",no_of_contours);

}

