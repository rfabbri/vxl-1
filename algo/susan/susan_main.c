
#include "susan.h"     /* general definition */
#include "susan_main.h"
#include "enlarge.h"
#include "edge_draw.h"
#include "susan_edges.h"  /* edge detection */
#include "susan_sub.h"
#include "susan_edges_small.h"  /* smaller 3x3 kernel */
#include "getint.h"     /* ?? */
//#include "susan_principle.h"  /* calculate SUSAN principle ??? */
#include "int_to_char.h"   /* int to chat */
//#include "susan_principle_small.h"  /* 3x3 kernel */
#include "median.h"   /* get the median value of 8 neighboring pixels */
#include "susan_smooth.h"  /* smoothing for what ??? */
#include "susan_thin.h"   /* thin the edge ?? */
#include "susan_corners.h"  /* corner detector */
//#include "susan_corners_quick.h"  /* 3x3 kernel */
#include "setup_brightness_lut.h"   /* look up table to brightness ?? */
#include "susan_sub_function.h"
#include "SusanInterface.h"
#define PI 3.1412

SusanEdges edges_to_return;

SusanEdges * susan_main(SusanImage_Gray8 *im_in,SusanInterface *edge_detection_properties)
{
  uchar  *in, *bp, *mid,*gradient,*tempin,*do_sym;
  char * angle_edges;
  //  SusanEdges * edges_to_return=0;

  
  struct Susansubpixels * start;
  struct Susanpixels * temp_c=0;
  int *A,*B,*W;
  float  dt=4.0;
  int    *r,
    argindex=3,
    bt=edge_detection_properties->threshold,
    principle=0,
    thin_post_proc=1,
    sub_pixel=1,
    max_no_corners=1850,
    max_no_edges=2650,
    max_no_edges_medium=1775,
    mode = 0, i,
    three_by_three=0,
    x_size, y_size;
  CORNER_LIST corner_list;
  struct Susanpixels *corners_list=0;
  int mask_size;

  x_size=edge_detection_properties->width;
  y_size=edge_detection_properties->height;
  r   = (int *) malloc(x_size * y_size * sizeof(int));
  gradient=(uchar *)malloc(x_size*y_size*sizeof(uchar));
  angle_edges=(char *)malloc(x_size*y_size*sizeof(char));
  do_sym=malloc(x_size*y_size*sizeof(uchar));

  in=im_in->data;

  mask_size=edge_detection_properties->mask_size;
  mode=edge_detection_properties->mode;
  sub_pixel = edge_detection_properties->sub_pixel;
  
  A   = (int *) malloc(x_size * y_size * sizeof(int));
  B   = (int *) malloc(x_size * y_size * sizeof(int));
  W   = (int *) malloc(x_size * y_size * sizeof(int));
  for(i=0;i<x_size * y_size;i++)
    {
      A[i]=0;
      B[i]=0;
      W[i]=0;
      angle_edges[i]=10;
      r[i]=0;
    } 
  start=0;

  mode =1;

  switch (mode)
    {
    case 0:
      // smoothing 
      setup_brightness_lut(&bp,bt,2);
      susan_smoothing(three_by_three,in,dt,x_size,y_size,bp);
      break;
      
    case 1:
      /* {{{ edges */

      setup_brightness_lut(&bp,bt,6);

      mid = (uchar *)malloc(x_size*y_size);
      memset (mid,100,x_size * y_size); // note not set to zero 
      memset (do_sym,100,x_size * y_size); // note not set to zero 
      printf("\n mask_size : %d",mask_size);

      if(mask_size==2)
  susan_edges(in,r,mid,bp,max_no_edges,x_size,y_size,A,B,W,do_sym,angle_edges);
      else if(mask_size==0)
  susan_edges_small(in,r,mid,bp,max_no_edges,x_size,y_size,A,B,W,do_sym,angle_edges);
      else if(mask_size==1)
  susan_edges_medium(in,r,mid,bp,max_no_edges_medium,x_size,y_size,A,B,W,do_sym,angle_edges);
      //bt=(int)(1.2*bt);
      //setup_brightness_lut(&bp,bt,6);
      //susan_angle_edges(in,r,mid,bp,max_no_edges,x_size,y_size,A,B,W,do_sym,angle_edges);
      if(edge_detection_properties->thin==1)
  {
    susan_thin(r,mid,x_size,y_size);
  }
      edges_to_return.pixels=edge_draw(in,mid,x_size,y_size,A,B,r,angle_edges); 
      
      if(sub_pixel)
  {
    edges_to_return.subpixels=susan_subpixel(in,bp,r,max_no_edges,mid,x_size,y_size,A,B,W,do_sym,angle_edges,mask_size);
  }

      edges_to_return.pixel_corners=susan_corners(in,r,bp,max_no_corners,corners_list,x_size,y_size);
      
      break;
    case 2:
      /* {{{ corners */
      
      r   = (int *) malloc(x_size * y_size * sizeof(int));
      setup_brightness_lut(&bp,bt,6);
      //susan_corners(in,r,bp,max_no_corners,corner_list,x_size,y_size);
      
      break;
   
    }
 
 
  printf("\n susan exit");
  //  edges_to_return=&edges;
  
  //  corners_list=edges_to_return.subpixels;

  free(gradient);
  free(r);
  free(do_sym);
  free(mid);
  free(A);
  free(B);
  free(W);
  return &edges_to_return;
}
