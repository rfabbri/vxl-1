#include "susan_smooth.h"

void susan_smoothing(int three_by_three, uchar *in, float dt, int x_size, int y_size, uchar *bp)

{
  /* {{{ vars */
  
  float temp;
  int   n_max, increment, mask_size,
    i,j,x,y,area,brightness,tmp,centre;
  uchar *ip, *dp, *dpt, *cp, *out=in,
    *tmp_image;
  TOTAL_TYPE total;
  
  /* }}} */
  
  /* {{{ setup larger image and border sizes */
  
  if (three_by_three==0)
    mask_size = ((int)(1.5 * dt)) + 1;
  else
    mask_size = 1;
  
  total=0.1; /* test for total's type */
  if ( (dt>15) && (total==0) )
    {
      printf("Distance_thresh (%f) too big for integer arithmetic.\n",dt);
      printf("Either reduce it to <=15 or recompile with variable \"total\"\n");
      printf("as a float: see top \"defines\" section.\n");
      exit(0);
    }
  
  if ( (mask_size>x_size) || (mask_size>y_size) )
    {
      printf("Mask size (1.5*distance_thresh+1=%d) too big for image (%dx%d).\n",mask_size,x_size,y_size);
      exit(0);
    }
  
  tmp_image = (uchar *) malloc( (x_size+mask_size*2) * (y_size+mask_size*2) );
  enlarge(&in,tmp_image,&x_size,&y_size,mask_size);
  
  /* }}} */
  
  if (three_by_three==0)
  {     /* large Gaussian masks */
    /* {{{ setup distance lut */

    n_max = (mask_size*2) + 1;
    
    increment = x_size - n_max;
    
    dp     = (unsigned char *)malloc(n_max*n_max);
    dpt    = dp;
    temp   = -(dt*dt);
    
    for(i=-mask_size; i<=mask_size; i++)
      for(j=-mask_size; j<=mask_size; j++)
  {
    x = (int) (100.0 * exp( ((float)((i*i)+(j*j))) / temp ));
    *dpt++ = (unsigned char)x;
  }
    
    /* }}} */
    /* {{{ main section */
    
    for (i=mask_size;i<y_size-mask_size;i++)
    {
      for (j=mask_size;j<x_size-mask_size;j++)
  {
    area = 0;
    total = 0;
    dpt = dp;
    ip = in + ((i-mask_size)*x_size) + j - mask_size;
    centre = in[i*x_size+j];
    cp = bp + centre;
    for(y=-mask_size; y<=mask_size; y++)
      {
        for(x=-mask_size; x<=mask_size; x++)
    {
      brightness = *ip++;
      tmp = *dpt++ * *(cp-brightness);
      area += tmp;
      total += tmp * brightness;
    }
        ip += increment;
      }
    tmp = area-10000;
    if (tmp==0)
      *out++=median(in,i,j,x_size);
    else
      *out++=((total-(centre*10000))/tmp);
  }
    }
  
  /* }}} */
  }
  else
    {     /* 3x3 constant mask */
      /* {{{ main section */
      
      for (i=1;i<y_size-1;i++)
  {
    for (j=1;j<x_size-1;j++)
      {
        area = 0;
        total = 0;
        ip = in + ((i-1)*x_size) + j - 1;
        centre = in[i*x_size+j];
        cp = bp + centre;
        
        brightness=*ip++; tmp=*(cp-brightness); area += tmp; total += tmp * brightness;
        brightness=*ip++; tmp=*(cp-brightness); area += tmp; total += tmp * brightness;
        brightness=*ip; tmp=*(cp-brightness); area += tmp; total += tmp * brightness;
        ip += x_size-2;
        brightness=*ip++; tmp=*(cp-brightness); area += tmp; total += tmp * brightness;
        brightness=*ip++; tmp=*(cp-brightness); area += tmp; total += tmp * brightness;
        brightness=*ip; tmp=*(cp-brightness); area += tmp; total += tmp * brightness;
        ip += x_size-2;
        brightness=*ip++; tmp=*(cp-brightness); area += tmp; total += tmp * brightness;
        brightness=*ip++; tmp=*(cp-brightness); area += tmp; total += tmp * brightness;
        brightness=*ip; tmp=*(cp-brightness); area += tmp; total += tmp * brightness;
      
        tmp = area-100;
        if (tmp==0)
    *out++=median(in,i,j,x_size);
        else
    *out++=(total-(centre*100))/tmp;
      }
  }
    }
}
