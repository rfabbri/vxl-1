#include "susan_edges.h"

void susan_edges(in,r,mid,bp,max_no,x_size,y_size,A,B,W,do_sym,angle_edges)
  uchar *in, *bp, *mid,* do_sym;
  int  *r, max_no, x_size, y_size;
  int *A,*B,*W;
  char * angle_edges;
{
  float z;
  int   do_symmetry, i, j, m, n, a, b, x, y, w;
  uchar c,*p,*cp;
  int xx,yy,sq;
  int *cgx,*cgy;
  float divide;
  int verbose=0;
  cgx=(int *)malloc(x_size*y_size*sizeof(int));
  cgy=(int *)malloc(x_size*y_size*sizeof(int));
  //memset (r,0,x_size * y_size * sizeof(int)); /* r[] = 0 */

  /* calculate the SUSAN response */
  for (i=3;i<y_size-3;i++)
    for (j=3;j<x_size-3;j++)
      {
  n=100;
  p=in + (i-3)*x_size + j - 1;
  cp=bp + in[i*x_size+j];

  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p);
  p+=x_size-3; 
  
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p);
  p+=x_size-5;
  
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p);
  p+=x_size-6;
  
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p);
  p+=2;
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p);
  p+=x_size-6;

  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p);
  p+=x_size-5;
  
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p);
  p+=x_size-3;
  
  n+=*(cp-*p++);
  n+=*(cp-*p++);
  n+=*(cp-*p);
  
  if (n<=max_no )    /* max_no = 2650 */
    r[i*x_size+j] = max_no - n;
      }
  
  for (i=4;i<y_size-4;i++)
    for (j=4;j<x_size-4;j++)
      {
  if (r[i*x_size+j]>0)
    {
      m=r[i*x_size+j];
      n=max_no - m;
      cp=bp + in[i*x_size+j];
      
      if (n>600 )
        {

        p=in + (i-3)*x_size + j - 1;
        x=0;y=0;
        
        c=*(cp-*p++);x-=c;y-=3*c;
        c=*(cp-*p++);y-=3*c;
        c=*(cp-*p);x+=c;y-=3*c;
        p+=x_size-3; 
        
        c=*(cp-*p++);x-=2*c;y-=2*c;
        c=*(cp-*p++);x-=c;y-=2*c;
        c=*(cp-*p++);y-=2*c;
        c=*(cp-*p++);x+=c;y-=2*c;
        c=*(cp-*p);x+=2*c;y-=2*c;
        p+=x_size-5;
        
        c=*(cp-*p++);x-=3*c;y-=c;
        c=*(cp-*p++);x-=2*c;y-=c;
        c=*(cp-*p++);x-=c;y-=c;
        c=*(cp-*p++);y-=c;
        c=*(cp-*p++);x+=c;y-=c;
        c=*(cp-*p++);x+=2*c;y-=c;
        c=*(cp-*p);x+=3*c;y-=c;
        p+=x_size-6;
        
        c=*(cp-*p++);x-=3*c;
        c=*(cp-*p++);x-=2*c;
        c=*(cp-*p);x-=c;
        p+=2;
        c=*(cp-*p++);x+=c;
        c=*(cp-*p++);x+=2*c;
        c=*(cp-*p);x+=3*c;
        p+=x_size-6;
        
        c=*(cp-*p++);x-=3*c;y+=c;
        c=*(cp-*p++);x-=2*c;y+=c;
        c=*(cp-*p++);x-=c;y+=c;
        c=*(cp-*p++);y+=c;
        c=*(cp-*p++);x+=c;y+=c;
        c=*(cp-*p++);x+=2*c;y+=c;
        c=*(cp-*p);x+=3*c;y+=c;
        p+=x_size-5;
        
        c=*(cp-*p++);x-=2*c;y+=2*c;
        c=*(cp-*p++);x-=c;y+=2*c;
        c=*(cp-*p++);y+=2*c;
        c=*(cp-*p++);x+=c;y+=2*c;
        c=*(cp-*p);x+=2*c;y+=2*c;
        p+=x_size-3;
        
        c=*(cp-*p++);x-=c;y+=3*c;
        c=*(cp-*p++);y+=3*c;
        c=*(cp-*p);x+=c;y+=3*c;
        
        z = sqrt((float)((x*x) + (y*y)));
        if (z > (0.9*(float)n)) /* 0.5 */
          {
      do_symmetry=0;
      
      if (x==0)
        z=1000000.0;
      else
        z=((float)y) / ((float)x);
      
      if (z < 0) {z=-z; w=-1;}
      else {w=1;}
      
      if(z<=0.27 && z>=0)
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
      
      if ( (m > r[(i+a)*x_size+j+b]) && (m >= r[(i-a)*x_size+j-b]) &&
           (m > r[(i+(2*a))*x_size+j+(2*b)]) && (m >= r[(i-(2*a))*x_size+j-(2*b)]) )
        {        
          mid[i*x_size+j] = 1;

        }
      A[i*x_size+j]=a;
      B[i*x_size+j]=b;
      W[i*x_size+j]=w;
          }
        else
          do_symmetry=1;

        }
      else 
        do_symmetry=1;
      
      do_sym[i*x_size+j]=do_symmetry;
      if (do_symmetry==1)
        { 
    p=in + (i-3)*x_size + j - 1;
    x=0; y=0; w=0;
        
        /*   |      \
       y  -x-  w
       |        \   */
        
        c=*(cp-*p++);x+=c;y+=9*c;w+=3*c;
        c=*(cp-*p++);y+=9*c;
        c=*(cp-*p);x+=c;y+=9*c;w-=3*c;
        p+=x_size-3; 
        
        c=*(cp-*p++);x+=4*c;y+=4*c;w+=4*c;
        c=*(cp-*p++);x+=c;y+=4*c;w+=2*c;
        c=*(cp-*p++);y+=4*c;
        c=*(cp-*p++);x+=c;y+=4*c;w-=2*c;
        c=*(cp-*p);x+=4*c;y+=4*c;w-=4*c;
        p+=x_size-5;
        
        c=*(cp-*p++);x+=9*c;y+=c;w+=3*c;
        c=*(cp-*p++);x+=4*c;y+=c;w+=2*c;
        c=*(cp-*p++);x+=c;y+=c;w+=c;
        c=*(cp-*p++);y+=c;
        c=*(cp-*p++);x+=c;y+=c;w-=c;
        c=*(cp-*p++);x+=4*c;y+=c;w-=2*c;
        c=*(cp-*p);x+=9*c;y+=c;w-=3*c;
        p+=x_size-6;
        
        c=*(cp-*p++);x+=9*c;
        c=*(cp-*p++);x+=4*c;
        c=*(cp-*p);x+=c;
        p+=2;
        c=*(cp-*p++);x+=c;
        c=*(cp-*p++);x+=4*c;
        c=*(cp-*p);x+=9*c;
        p+=x_size-6;
        
        c=*(cp-*p++);x+=9*c;y+=c;w-=3*c;
        c=*(cp-*p++);x+=4*c;y+=c;w-=2*c;
        c=*(cp-*p++);x+=c;y+=c;w-=c;
        c=*(cp-*p++);y+=c;
        c=*(cp-*p++);x+=c;y+=c;w+=c;
        c=*(cp-*p++);x+=4*c;y+=c;w+=2*c;
        c=*(cp-*p);x+=9*c;y+=c;w+=3*c;
        p+=x_size-5;
        
        c=*(cp-*p++);x+=4*c;y+=4*c;w-=4*c;
        c=*(cp-*p++);x+=c;y+=4*c;w-=2*c;
        c=*(cp-*p++);y+=4*c;
        c=*(cp-*p++);x+=c;y+=4*c;w+=2*c;
        c=*(cp-*p);x+=4*c;y+=4*c;w+=4*c;
        p+=x_size-3;
        
        c=*(cp-*p++);x+=c;y+=9*c;w-=3*c;
        c=*(cp-*p++);y+=9*c;
        c=*(cp-*p);x+=c;y+=9*c;w+=3*c;
        
        if(x==0 && y==0 && n==100)
    angle_edges[i*x_size+j]=9;
        else
        {
    
    if (y==0)
      {
        z = 1000000.0;
      }
    else
      z = ((float)x) / ((float)y);

  
    if(w==0 && z<=1)
      angle_edges[i*x_size+j]=4;
    else if(w==0 && z>1)
      angle_edges[i*x_size+j]=0;
    else
    {
      if(z<=0.21)
        {
          
          if(verbose==1)
      printf("\n 90degree ");
          angle_edges[i*x_size+j]=4;
        }
      else if(z<=0.62)
        {

          if(verbose==1)
      printf("\n 67.5 degree %f",atan(z));
          if(w>0)
      angle_edges[i*x_size+j]=5;
          else 
       angle_edges[i*x_size+j]=3;
        }
      else if(z<=1.53)
        {
          if(verbose==1)
      printf("\n 45 degree");

          
          if(w>0)
      angle_edges[i*x_size+j]=6;
          else 
      {
        angle_edges[i*x_size+j]=2;
      }
          }
      else if(z<=5.14)
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
      }
      if (z < 0.5) { /* vertical */ a=0; b=1; }
      else { if (z > 2.0) { /* horizontal */ a=1; b=0; }
      else { /* diagonal */ if (w>0) { a=-1; b=1; }
      else { a=1; b=1; }}}
        
      A[i*x_size+j]=a;
      B[i*x_size+j]=b;
      W[i*x_size+j]=w;
      if ( (m > r[(i+a)*x_size+j+b]) && (m >= r[(i-a)*x_size+j-b]) &&
           (m > r[(i+(2*a))*x_size+j+(2*b)]) && (m >= r[(i-(2*a))*x_size+j-(2*b)]) )
        {
          mid[i*x_size+j] = 2;        
          
        }
      
        }
    
      }
      
    }
  
  }
  
}
