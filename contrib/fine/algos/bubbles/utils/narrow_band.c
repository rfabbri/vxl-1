#include <stdio.h>
#include <math.h>

#define ENDOFNBANDEDGES -10

void narrow_band_boundaries(float *surface_array, float *level_array, int *nband_edges, 
                            float band_size, int height, int width)

{
  int y,x,pos,ii;

  ii=0;
  for(y=1; y<(height-1); y++) {
    for(x=1; x<(width-1); x++) {
      pos = y*width+x;
      if ((level_array[pos] == band_size && surface_array[pos] < 0.0)) {
        nband_edges[ii] = y; 
        nband_edges[ii+1] = x;
        ii+=2;
      }
    }
  }
  
  nband_edges[ii] = ENDOFNBANDEDGES; ii++;
  nband_edges[ii+1] = ENDOFNBANDEDGES; ii++;
 
}


int zero_level_in_narrow_band(float *level_array, int *nband_edges, float level, 
                              int height, int width)

{
  int i,y,x,yy,xx,flag,j,pos;
  FILE *fopen(), *f1;
  
  i=0; flag = 0;
  while(nband_edges[i] != ENDOFNBANDEDGES) {
    y = nband_edges[i];
    x = nband_edges[i+1];
    pos= y*width+x;
    if (level_array[pos] == level) {
      flag = 1;
      /* printf("danger at y =%i and x =%i and surface = %lf\n",   
        y,x, level_array[pos]); */
      break;
    }
    i++; i++;
  }
  return(flag);
}

              










