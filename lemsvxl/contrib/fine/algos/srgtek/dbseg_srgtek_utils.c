#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include "dbseg_srgtek_utils.h"
#define irint(x) (int) (x+0.5)

void CopyUByteToUByte(unsigned char *bimage,unsigned char *dimage,
              int depth,int height,int width)
{
  int zz,yy,xx,pos;
  
  pos=0;
  for(zz=0; zz<depth; zz++) {
    for(yy = 0;yy < height; yy++){
      for(xx = 0;xx < width;xx++){
    dimage[pos] = (unsigned char) bimage[pos];
    pos++; 
      }
    }
  }
}

void CopyUByteToInt(unsigned char *bimage,int *dimage,
              int depth,int height,int width)
{
  int zz,yy,xx,pos;
  
  pos=0;
  for(zz=0; zz<depth; zz++) {
    for(yy = 0;yy < height; yy++){
      for(xx = 0;xx < width;xx++){
    dimage[pos] = (int) bimage[pos];
    pos++; 
      }
    }
  }
}

void CopyIntToUByte(int *bimage,unsigned char *dimage,
              int depth,int height,int width)
{
  int zz,yy,xx,pos;
  
  pos=0;
  for(zz=0; zz<depth; zz++) {
    for(yy = 0;yy < height; yy++){
      for(xx = 0;xx < width;xx++){
    dimage[pos] = (unsigned char) bimage[pos];
    pos++; 
      }
    }
  }
}


void CopyUByteToDouble(unsigned char *bimage,double *dimage,int depth,int height,int width)
{
  int zz,yy,xx,pos;
  
  pos=0;
  for(zz=0; zz<depth; zz++) {
    for(yy = 0;yy < height; yy++){
      for(xx = 0;xx < width;xx++){
    dimage[pos] = (double)bimage[pos];
    pos++; 
      }
    }
  }
}

void CopyUByteToFloat(unsigned char *bimage,float *dimage,int depth,int height,int width)
{
  int zz,yy,xx,pos;
  
  pos=0;
  for(zz=0; zz<depth; zz++) {
    for(yy = 0;yy < height; yy++){
      for(xx = 0;xx < width;xx++){
    dimage[pos] = (float) bimage[pos];
    pos++; 
      }
    }
  }
}

void CopyUShortToDouble(unsigned short *bimage,double *dimage,int depth,int height,int width)
{
  int zz,yy,xx,pos;
  
  pos=0;
  for(zz=0; zz<depth; zz++) {
    for(yy=0; yy<height; yy++){
      for(xx=0; xx<width; xx++){
    dimage[pos] = (double) bimage[pos];
    pos++;
      }
    }
  }
}

void CopyUShortToFloat(unsigned short *bimage,float *dimage,int depth,int height,int width)
{
  int zz,yy,xx,pos;
  
  pos=0;
  for(zz=0; zz<depth; zz++) {
    for(yy = 0;yy < height; yy++){
      for(xx = 0;xx < width;xx++){
    dimage[pos] = (float) bimage[pos];
    pos++; 
      }
    }
  }
}
    
void CopyDoubleToUByte(double *dimage,unsigned char *bimage,int depth,int height,int width)
{
  int zz,yy,xx;
  int dvalue,pos;
  
  pos=0;
  for(zz=0; zz<depth; zz++) {
    for(yy=0; yy<height; yy++){
      for(xx=0; xx<width; xx++){
    
    /** round the floating point to the nearest integer and then check
      that it is in the valid range for unsigned byte **/
    
    dvalue = irint(dimage[pos]); 
    if (dvalue < 0){
      dvalue = 0;
    }
    else if (dvalue > 255)
      {
        dvalue = 255;
      }
    bimage[pos] = (unsigned char)dvalue;
    pos++;
      }
    }
  }
}

void CopyDoubleToFloat(double *dimage,float *fimage,int depth,int height,int width)
{
  int zz,yy,xx,pos;
  
  pos =0;
  for(zz=0; zz<depth; zz++) {
    for(yy=0; yy<height; yy++){
      for(xx=0; xx<width; xx++){
    fimage[pos] = (float) dimage[pos];
    pos++;
      }
    }
  }
}

void CopyFloatToDouble(float *fimage,double *dimage,int depth,int height,int width)
{
 int zz,yy,xx,pos;
  
 pos =0;
 for(zz=0; zz<depth; zz++) {
   for(yy=0; yy<height; yy++){
     for(xx=0; xx<width; xx++){
       dimage[pos] = (double) fimage[pos];
       pos++;
     }
   }
 }
}

void CopyDoubleToUShort(double *dimage,unsigned short *uimage,int depth,int height,int width)
{
  int zz, yy,xx,pos;

  pos =0;
  for(zz=0; zz<depth; zz++) {
    for(yy = 0;yy < height; yy++){
      for(xx = 0;xx < width; xx++){
    uimage[pos] = (unsigned short) dimage[pos];
    pos++;
      }
    }
  }
  
}

void initialize_double_image (double *array, double value, int height, int width)

{
  int y,x,pos;
  pos =0;
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
     array[pos] = value;
     pos++;
   }
  }
}


void copy_image (double *surface_array, double *aux_surface_array, int depth,
         int height, int width)

{
  int z, y, x, position;

  position = 0;
  for(z=0; z<depth; z++) {
    for(y=0; y<height; y++) {
      for(x=0; x<width; x++) {
    aux_surface_array[position] = surface_array[position];
    position++;
      }
    }
  }
}

double find_norm (double *surface_array, double *aux_surface_array, int height, int width)

{ 
  double norm, new_value, old_value;
  int y, x, position;
  
  position = 0;
  norm = 0.0;
  for(y=0; y<height; y++) {
   for(x=0; x<width; x++)  {
    new_value = surface_array[position];
    old_value = aux_surface_array[position];
    norm += (sqrt ((new_value-old_value)*(new_value-old_value)));
    position++; 
  }
 }
 return(norm);
}
 
 /* create the initial bubbles based on the region information */ 

void initialize_bubbles (double *deviation_array, double *image_array, double thresh,
                         int height, int width)

{ 
  double deviation;
  int y,x,position;
  
  position =0; 
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      deviation = deviation_array[position];
      if (deviation < thresh) 
    image_array[position] = 0.0;
      else image_array[position] = 255.0;
      position++;
    }
  }
}
    

void good_bubbles (double *image_array, double *speed_array, double thresh,
                   int height, int width)

{
  double speed_value;
  int y,x,position;
 
  position =0; 
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      speed_value = speed_array[position];
      if (speed_value > thresh) 
    image_array[position] = 255.0;
      position++;
    }
  }
}


double min(double val1, double val2, double val3, double val4, double val5)
     
{
  double val;
  val = 10001.0;
  if (val1 < val) val = val1;
  if (val2 < val) val = val2;
  if (val3 < val) val = val3;
  if (val4 < val) val = val4;
  if (val5 < val) val = val5;
  return(val);
}
 
void make_slice (double *array_3d, double *array_2d, int slice_direction, int slice,
                 int depth, int height, int width)
     
{
  int z,y,x;
  
  /* z slice */
  if (slice_direction == 1) {
    for (y=0; y<height; y++) {
      for (x=0; x<width; x++) {
    array_2d[y*width+x] = array_3d[slice*height*width+y*width+x];
      }
    }
  }
  
  /* y slice  */ 
  
  if (slice_direction == 2) {
    for (z=0; z<depth; z++) {
      for (x=0; x<width; x++) {
    array_2d[z*width+x] = array_3d[z*height*width+slice*width+x];
      }
    }
  }

  /* x slice */ 
  if (slice_direction == 3) {
    for (z=0; z<depth; z++) {
      for (y=0; y<height; y++) {
    array_2d[z*height+y] = array_3d[z*height*width+y*width+slice];
      }
    }
  }
}

double image_element_substitute (int x, int y, double *image,int height,int width)
 
         
{
  int xx,yy;

  if (x < 0)           xx = 0;
  else if (x < width)  xx = x;
  else                 xx = width - 1;

  if (y < 0)           yy = 0;
  else if (y < height) yy = y;
  else                 yy = height - 1;

  return(image[yy*width + xx]);  /* this is in keeping with */
}                 /* image-element+substitute in */
   
int pixelread(int x,int y, int width, unsigned char *contour_array)
{
  return(contour_array[y*width + x]);
}

void pixelwrite(int x,int y,int width, int nv,unsigned char *contour_array)
{
  contour_array[y*width + x] = nv;
}

int invert_image(int width, int height, unsigned char *contour_array)
{

  int y,x,size,pos;
 
  size =0;
  pos =0; 
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      if (contour_array[pos] == 0)
        {contour_array[pos] = 1;}
      else
        {contour_array[pos] = 0;
         size++;}
      pos++;
    }
  }
return(size); 
}

#ifdef __cplusplus
}
#endif





