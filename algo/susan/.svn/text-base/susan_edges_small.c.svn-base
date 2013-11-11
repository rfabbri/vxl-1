#include "susan_edges_small.h"

void susan_edges_small(in,r,mid,bp,max_no,x_size,y_size,A,B,W,do_sym,angle_edges)
  uchar *in, *bp, *mid,*do_sym;
  char *A,*B,*W;
  int   *r, max_no, x_size, y_size;
  char * angle_edges;
{
  float z;
  int   do_symmetry, i, j, m, n, a, b, x, y, w;
  uchar c,*p,*cp;
  int verbose=0;

  memset (r,0,x_size * y_size * sizeof(int));
  max_no = 730; /* ho hum ;) */

  for (i=1;i<y_size-1;i++)
    for (j=1;j<x_size-1;j++)
    {
      n=100;
      p=in + (i-1)*x_size + j - 1;
      cp=bp + in[i*x_size+j];

      n+=*(cp-*p++);
      n+=*(cp-*p++);
      n+=*(cp-*p);
      p+=x_size-2; 

      n+=*(cp-*p);
      p+=2;
      n+=*(cp-*p);
      p+=x_size-2;

      n+=*(cp-*p++);
      n+=*(cp-*p++);
      n+=*(cp-*p);

      if (n<=max_no)
        r[i*x_size+j] = max_no - n;
    }

  for (i=2;i<y_size-2;i++)
    for (j=2;j<x_size-2;j++)
    {
      if (r[i*x_size+j]>0)
      {
        m=r[i*x_size+j];
        n=max_no - m;
        cp=bp + in[i*x_size+j];

        if (n>250)
        {
          p=in + (i-1)*x_size + j - 1;
          x=0;y=0;

          c=*(cp-*p++);x-=c;y-=c;
          c=*(cp-*p++);y-=c;
          c=*(cp-*p);x+=c;y-=c;
          p+=x_size-2; 

          c=*(cp-*p);x-=c;
          p+=2;
          c=*(cp-*p);x+=c;
          p+=x_size-2;

          c=*(cp-*p++);x-=c;y+=c;
          c=*(cp-*p++);y+=c;
          c=*(cp-*p);x+=c;y+=c;

          z = sqrt((float)((x*x) + (y*y)));
          if (z > (0.4*(float)n)) /* 0.6 */
          {
            do_symmetry=0;
            if (x==0)
              z=1000000.0;
            else
              z=((float)y) / ((float)x);

            if (z < 0) { z=-z; w=-1; }
            else w=1;

      if(z<=0.27)
        {
    if(verbose==1)
      printf("\n 90degree %f",atan(z));
    angle_edges[i*x_size+j]=4;
        }
      else if(z<=0.57)
        {
    if(verbose==1)
      printf("\n 67.5 degree %f",atan(z));
    if(w>0)
      angle_edges[i*x_size+j]=3;
    else
      angle_edges[i*x_size+j]=5;
        }
      else if(z<=1.73)
        {
    if(verbose==1)
      printf("\n 45 degree");
    if(w>0)
      angle_edges[i*x_size+j]=2;
    else
      angle_edges[i*x_size+j]=6;
        }
      else if(z<=3.73)
        {
    if(verbose==1)
      printf("\n 22.5 degree");
    if(w>0)
      angle_edges[i*x_size+j]=1;
    else
      angle_edges[i*x_size+j]=7;
        }
      else
        {  
    if(verbose==1)
      printf("\n 0 degree");
    angle_edges[i*x_size+j]=0;
        }
            if (z < 0.5) { /* vert_edge */ a=0; b=1; }
            else { if (z > 2.0) { /* hor_edge */ a=1; b=0; }
            else { /* diag_edge */ if (w>0) { a=1; b=1; }
                                   else { a=-1; b=1; }}}
            if ( (m > r[(i+a)*x_size+j+b]) && (m >= r[(i-a)*x_size+j-b]) )
              mid[i*x_size+j] = 1;
          }
          else
            do_symmetry=1;
        }
        else
          do_symmetry=1;

  do_sym[i*x_size+j]=do_symmetry;

        if (do_symmetry==1)
        { 
          p=in + (i-1)*x_size + j - 1;
          x=0; y=0; w=0;

          /*   |      \
               y  -x-  w
               |        \   */

          c=*(cp-*p++);x+=c;y+=c;w+=c;
          c=*(cp-*p++);y+=c;
          c=*(cp-*p);x+=c;y+=c;w-=c;
          p+=x_size-2; 

          c=*(cp-*p);x+=c;
          p+=2;
          c=*(cp-*p);x+=c;
          p+=x_size-2;

          c=*(cp-*p++);x+=c;y+=c;w-=c;
          c=*(cp-*p++);y+=c;
          c=*(cp-*p);x+=c;y+=c;w+=c;

          if (y==0)
            z = 1000000.0;
          else
      z = ((float)x) / ((float)y);
  
    
    if(z<=0.27)
      {
        if(verbose==1)
    printf("\n 90degree %f",atan(z));
        angle_edges[i*x_size+j]=4;
      }
    else if(z<=0.57)
      {
        if(verbose==1)
    printf("\n 67.5 degree %f",atan(z));
        if(w>0)
    angle_edges[i*x_size+j]=5;
        else
    angle_edges[i*x_size+j]=3;
      }
    else if(z<=1.73)
      {
        if(verbose==1)
    printf("\n 45 degree");
        if(w>0)
    angle_edges[i*x_size+j]=6;
        else
    angle_edges[i*x_size+j]=2;
      }
    else if(z<=3.73)
      {
        if(verbose==1)
    printf("\n 22.5 degree");
        if(w>0)
    angle_edges[i*x_size+j]=7;
        else
    angle_edges[i*x_size+j]=1;
      }
    else
      {  
        if(verbose==1)
    printf("\n 0 degree");
        angle_edges[i*x_size+j]=0;
      }
          if (z < 0.5) { /* vertical */ a=0; b=1; }
          else { if (z > 2.0) { /* horizontal */ a=1; b=0; }
          else { /* diagonal */ if (w>0) { a=-1; b=1; }
                                else { a=1; b=1; }}}
    A[i*x_size+j]=a;
    B[i*x_size+j]=b;
    W[i*x_size+j]=w;
    if ( (m > r[(i+a)*x_size+j+b]) && (m >= r[(i-a)*x_size+j-b]) )
            mid[i*x_size+j] = 2;        
        }
      }
    }
}
