#include "median.h"

uchar median(in,i,j,x_size)  /* i = row, j = col */
  uchar *in;
  int   i, j, x_size;
{
int p[8],k,l,tmp;

  p[0]=in[(i-1)*x_size+j-1]; 
  p[1]=in[(i-1)*x_size+j  ];
  p[2]=in[(i-1)*x_size+j+1];
  p[3]=in[(i  )*x_size+j-1];
  p[4]=in[(i  )*x_size+j+1];
  p[5]=in[(i+1)*x_size+j-1];
  p[6]=in[(i+1)*x_size+j  ];
  p[7]=in[(i+1)*x_size+j+1];

  /* bubble sort */
  for(k=0; k<7; k++)
    for(l=0; l<(7-k); l++)
      if (p[l]>p[l+1])
      {
        tmp=p[l]; p[l]=p[l+1]; p[l+1]=tmp;
      }
  /* get the median value, not the mean value */
  return( (p[3]+p[4]) / 2 );
}
