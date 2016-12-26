#include "edge_draw.h"
#define PI 3.1412

struct Susanpixels * edge_draw(in,mid,x_size,y_size,a,b,gradient,angle_edges)
  uchar *in, *mid;
  int x_size, y_size;
  int *a,*b;
  uchar * gradient;
  char * angle_edges;
{
  int   i,j;
  uchar min,max;
  struct Susanpixels * start_edges=0;
  struct Susanpixels * temp_edges=0,*stemp_edges=0;
  min=3700;
  max=0;
  start_edges=0;
   for (i=0; i<x_size*y_size; i++)
     {
      if (mid[i]<8) 
  {
    if(angle_edges[i]<10)
      {    
        temp_edges=(struct Susanpixels *)malloc(sizeof(struct Susanpixels ));
        j=i%x_size;
        temp_edges->ypos=floor(i/x_size);
        temp_edges->xpos=j;
        temp_edges->direction=(char) angle_edges[i];//((atan2(b[i],(a[i]+0.0001))/PI)*255);
        temp_edges->strength=(float)(gradient[i]);
       
        temp_edges->next=0;
        if(start_edges==0)
    {
      stemp_edges=temp_edges;
      start_edges=stemp_edges;
    }
        else
    {
      stemp_edges->next=temp_edges;
      stemp_edges=stemp_edges->next;
    }
      }
  }
     
    } 
   return start_edges;
}
