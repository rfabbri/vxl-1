#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dbseg_jseg_mathutil.h"
#include "dbseg_jseg_imgutil.h"

void yuv2rgb(unsigned char *img,int size)
{
  int crv,cbu,cgu,cgv,r,g,b,y,u,v,i;

  crv = 117504;
  cbu = 138453;
  cgu = 13954;
  cgv = 34903;
/*
  crv = 104597;
  cbu = 132201;
  cgu = 25675;
  cgv = 53279;
*/

  for (i=0;i<size;i+=3)
  {
    y = 76309 * (img[i]-16);
    u = img[i+1] - 128;
    v = img[i+2] - 128;
    r = (y +         crv*v + 32768)>>16;
    g = (y - cgu*u - cgv*v + 32768)>>16;
    b = (y + cbu*u         + 32786)>>16;
    if (r>255) r=255; if (r<0) r=0;
    if (g>255) g=255; if (g<0) g=0;
    if (b>255) b=255; if (b<0) b=0;
    img[i] = (unsigned char) r;
    img[i+1] = (unsigned char) g;
    img[i+2] = (unsigned char) b;
  }
}

void rgb2yuv(unsigned char *img,int size)
{
  int r,g,b,i;
  double y,u,v;

  for (i=0;i<size;i+=3)
  {
    r = (int) img[i];
    g = (int) img[i+1];
    b = (int) img[i+2];
    r = (r<<16) - 32768;
    g = (g<<16) - 32768;
    b = (b<<16) - 32768;
    y = ( 0.2124997*r + 0.7153988*g + 0.0721015*b )/76309 + 16;
    u = (-0.1171202*r - 0.3942953*g + 0.5114155*b )/76309 + 128;
    v = ( 0.5114155*r - 0.4645915*g - 0.0468239*b )/76309 + 128;
/*
    y = ( 0.2990011*r + 0.5869971*g + 0.1140018*b )/76309 + 16;
    u = (-0.1725893*r - 0.3388262*g + 0.5114155*b )/76309 + 128;
    v = ( 0.5114155*r - 0.4282452*g - 0.0831703*b )/76309 + 128;
*/

    if (y>255) y=255; if (y<0) y=0;
    if (u>255) u=255; if (u<0) u=0;
    if (v>255) v=255; if (v<0) v=0;

    img[i] = (unsigned char) round(y);
    img[i+1] = (unsigned char) round(u);
    img[i+2] = (unsigned char) round(v);
  }
}

void rgb2luv(unsigned char *RGB,float *LUV,int size)
{
  int i;
  double x,y,X,Y,Z,den,u2,v2,X0,Z0,Y0,u20,v20,r,g,b;

  X0 = (0.607+0.174+0.201);
  Y0 = (0.299+0.587+0.114);
  Z0 = (      0.066+1.117);

/* Y0 = 1.0 */
  u20 = 4*X0/(X0+15*Y0+3*Z0);
  v20 = 9*Y0/(X0+15*Y0+3*Z0);

  for (i=0;i<size;i+=3)
  {
    if (RGB[i]<=20)  r=(double) (8.715e-4*RGB[i]);
    else r=(double) pow((RGB[i]+25.245)/280.245, 2.22);

    if (RGB[i+1]<=20)  g=(double) (8.715e-4*RGB[i+1]);
    else g=(double) pow((RGB[i+1]+25.245)/280.245, 2.22);

    if (RGB[i+2]<=20)  b=(double) (8.715e-4*RGB[i+2]);
    else b=(double) pow((RGB[i+2]+25.245)/280.245, 2.22);

    X = 0.412453*r + 0.357580*g + 0.180423*b;
    Y = 0.212671*r + 0.715160*g + 0.072169*b;
    Z = 0.019334*r + 0.119193*g + 0.950227*b;

    if (X==0.0 && Y==0.0 && Z==0.0)
    {
      x=1.0/3.0; y=1.0/3.0;
    }
    else
    {
      den=X+Y+Z;
      x=X/den; y=Y/den;
    }

    den=-2*x+12*y+3;
    u2=4*x/den;
    v2=9*y/den;

    if (Y>0.008856) LUV[i] = (float) (116*pow(Y,1.0/3.0)-16);
    else LUV[i] = (float) (903.3*Y);
    LUV[i+1] = (float) (13*LUV[i]*(u2-u20));
    LUV[i+2] = (float) (13*LUV[i]*(v2-v20));
  }
}

void luv2rgb(unsigned char *RGB,float *LUV,int size)
{
  int i,k;
  double x,y,X,Y,Z,den,u2,v2,X0,Z0,Y0,u20,v20,vec[3];

  X0 = (0.607+0.174+0.201);
  Y0 = (0.299+0.587+0.114);
  Z0 = (      0.066+1.117);

/* Y0 = 1.0 */
  u20 = 4*X0/(X0+15*Y0+3*Z0);
  v20 = 9*Y0/(X0+15*Y0+3*Z0);

  for (i=0;i<size;i+=3)
  {
    if (LUV[i]>0)
    {
      if (LUV[i]<8.0) Y=((double) LUV[i])/903.3;
      else Y=pow(( ((double) LUV[i]) +16)/116.0,3.0);
      u2=((double) LUV[i+1])/13.0/((double) LUV[i])+u20;
      v2=((double) LUV[i+2])/13.0/((double) LUV[i])+v20;

      den = 6+3*u2-8*v2;
      if (den<0) printf("den<0\n");
      if (den==0) printf("den==0\n");
      x = 4.5*u2/den;
      y = 2.0*v2/den;

      X=x/y*Y;
      Z=(1-x-y)/y*Y;
    }
    else { X=0.0; Y=0.0; Z=0.0; }

    vec[0] = ( 3.240479*X-1.537150*Y-0.498536*Z);
    vec[1] = (-0.969256*X+1.875992*Y+0.041556*Z);
    vec[2] = ( 0.055648*X-0.204043*Y+1.057311*Z);
    for (k=0;k<3;k++)
    {
      if  (vec[k]<=0.018) vec[k] = 255*4.5*vec[k];
      else vec[k] = 255*(1.099*pow(vec[k],0.45)-0.099);
      if (vec[k]>255) vec[k] = 255;
      else if (vec[k]<0) vec[k] = 0;
      RGB[i+k] = (unsigned char) round(vec[k]);
    }
  }
}

void extendbounduc(unsigned char *img,unsigned char *imgout,int ny,int nx,int offset,int dim)
{
  int iy,ix,i,j,sy,ey,sx,ex,nx2,jump,loc;
  unsigned char f1,f2;

  nx2 = nx+2*offset;
  sy = offset; ey = ny+offset;
  sx = offset; ex = nx+offset;
  jump = 2*offset*dim;

  i = 0;
  loc = LOC(sy,sx,0,nx2,dim);
  for (iy=sy;iy<ey;iy++)
  {
    for (ix=sx;ix<ex;ix++)
    {
      for (j=0;j<dim;j++)
        imgout[loc++]=img[i++];
    }
    loc += jump;
  }

  for (ix=sx;ix<ex;ix++)
  {
    for (j=0;j<dim;j++)
    {
      f1 = imgout[LOC(offset,ix,j,nx2,dim)];
      f2 = imgout[LOC(ny+offset-1,ix,j,nx2,dim)];
      for (iy=0;iy<offset;iy++)
      {
        imgout[LOC(offset-1-iy,ix,j,nx2,dim)]  = f1;
        imgout[LOC(ny+offset+iy,ix,j,nx2,dim)] = f2;
      }
    }
  }

  ey = ny+2*offset;
  for (iy=0;iy<ey;iy++)
  {
    for (j=0;j<dim;j++)
    {
      f1 = imgout[LOC(iy,offset,j,nx2,dim)];
      f2 = imgout[LOC(iy,nx+offset-1,j,nx2,dim)];
      for (ix=0;ix<offset;ix++)
      {
        imgout[LOC(iy,offset-1-ix,j,nx2,dim)]  = f1;
        imgout[LOC(iy,nx+offset+ix,j,nx2,dim)] = f2;
      }
    }
  }
}

void extendboundfloat(float *img,float *imgout,int ny,int nx,int offset,int dim)
{
  int iy,ix,i,j,sy,ey,sx,ex,nx2,jump,loc;
  float f1,f2;

  nx2 = nx+2*offset;
  sy = offset; ey = ny+offset; 
  sx = offset; ex = nx+offset;
  jump = 2*offset*dim;

  i = 0;
  loc = LOC(sy,sx,0,nx2,dim);
  for (iy=sy;iy<ey;iy++)
  {
    for (ix=sx;ix<ex;ix++)
    {
      for (j=0;j<dim;j++)
        imgout[loc++]=img[i++];
    }
    loc += jump;
  }

  for (ix=sx;ix<ex;ix++)
  {
    for (j=0;j<dim;j++)
    {
      f1 = imgout[LOC(offset,ix,j,nx2,dim)];
      f2 = imgout[LOC(ny+offset-1,ix,j,nx2,dim)];
      for (iy=0;iy<offset;iy++)
      {
        imgout[LOC(offset-1-iy,ix,j,nx2,dim)]  = f1;
        imgout[LOC(ny+offset+iy,ix,j,nx2,dim)] = f2;
      }
    }
  }

  ey = ny+2*offset;
  for (iy=0;iy<ey;iy++)
  {
    for (j=0;j<dim;j++)
    {
      f1 = imgout[LOC(iy,offset,j,nx2,dim)];
      f2 = imgout[LOC(iy,nx+offset-1,j,nx2,dim)];
      for (ix=0;ix<offset;ix++)
      {
        imgout[LOC(iy,offset-1-ix,j,nx2,dim)]  = f1;
        imgout[LOC(iy,nx+offset+ix,j,nx2,dim)] = f2;
      }
    }
  }
}


#ifdef __cplusplus
}
#endif







