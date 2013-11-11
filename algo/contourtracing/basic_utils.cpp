/*------------------------------------------------------------*\
 * File: basic_utils.c
 *
 * Functions:
 *  CopyIm_UCharToFloat
 *  CopyIm_FloatToDouble
 *  PadIm_UCharToFloat
 *  PadIm_UCharToUChar
 *  CopyIm_UCharToInt
 *  CopyIm_UCharToUChar
 *  CopyIm_UShortToFloat
 *  PadIm_UShortToFloat
 *  CopyZLevelToUByte
 *  CopyIm_FloatToFloat
 *  CopyIm_FloatToUChar
 *  CopyIm_FloatToUShort
 *  InitializeImage_Float
 *  InitializeImage_UChar
 *  min_of_5
 *  image_element_substitute
 *  image_element_substitute_int
 *  image_element_substitute_double
 *  adjust_boundaries_float
 *  adjust_boundaries_double
 *  adjust_boundaries_int
 *  adjust_boundaries_uchar
 *  keymatch
 *  print_heap
 *  sign_function
 *  sqr
 *  mod
 *  image_extremas_float
 *  image_extremas_int
 *  image_extremas_UChar
 *  image_scale_UChar
 *  set_image_UChar
 *  decode_2d_location
 *  decode_3d_location
 *
 * History:
 *  #0:  by H.Tek
 *  #1: 25/02/98: Added CopyIm3d_UCharToDouble, F.Leymarie
 *  #2: 18/03/98: Moved CopyIm3d_UCharToDouble to "3dUtils.c", F.Leymarie
\*------------------------------------------------------------*/

#include <vcl_cmath.h>
#include <stdio.h>

#include "utils.h"


void
CopyIm_UCharToFloat(unsigned char *fromimage, float *intoimage, int height, int width)
{
  int yy,xx,pos;
  
  for(yy=0; yy<height; yy++){
    for(xx=0; xx<width; xx++){
      pos = yy*width+xx;
      intoimage[pos] = (float) fromimage[pos];
    }
  }
}

void
CopyIm_FloatToDouble(float *fromimage, double *intoimage, int height, int width)
{
  int yy,xx,pos;
  
  for(yy=0; yy<height; yy++){
    for(xx=0; xx<width; xx++){
      pos = yy*width+xx;
      intoimage[pos] = (double) fromimage[pos];
    }
  }
}
void
CopyIm_DoubleToDouble(double *fromimage, double *intoimage, int height, int width)
{
  int yy,xx,pos;
  
  for(yy=0; yy<height; yy++){
    for(xx=0; xx<width; xx++){
      pos = yy*width+xx;
      intoimage[pos] = (double) fromimage[pos];
    }
  }
}

void
PadIm_UCharToFloat(unsigned char *fromimage, float *intoimage, int height, int width)
{
  int yy,xx,pos;
  
  for(yy=1; yy<(height-1); yy++){
    for(xx=1; xx<(width-1); xx++){
      pos = yy*width+xx;
      intoimage[pos] = (float) fromimage[(yy-1)*(width-2)+(xx-1)];
    }
  }
  adjust_boundaries_float(intoimage,height,width);
}

void
PadIm_UCharToDouble(unsigned char *fromimage, double *intoimage, int height, int width)
{
  int yy,xx,pos;
  
  for(yy=1; yy<(height-1); yy++){
    for(xx=1; xx<(width-1); xx++){
      pos = yy*width+xx;
      intoimage[pos] = (double) fromimage[(yy-1)*(width-2)+(xx-1)];
    }
  }
  adjust_boundaries_double(intoimage,height,width);
}

void
PadIm_UCharToUChar(unsigned char *fromimage, unsigned char*intoimage, 
       int height, int width)
{
  int yy,xx,pos;
  
  for(yy=1; yy<(height-1); yy++){
    for(xx=1; xx<(width-1); xx++){
      pos = yy*width+xx;
      intoimage[pos] =  fromimage[(yy-1)*(width-2)+(xx-1)];
    }
  }
  adjust_boundaries_uchar(intoimage,height,width);
}

void
CopyIm_UCharToInt(unsigned char *fromimage, int *intoimage, int height, int width)
{
  int yy,xx,pos;
  
  for(yy=0; yy<height; yy++){
    for(xx=0; xx<width; xx++){
      pos = yy*width+xx;
      intoimage[pos] = (int) fromimage[pos];
    }
  }

}

void
CopyIm_UCharToUChar(unsigned char *fromimage,unsigned char *intoimage,
        int height,int width)
{
  int yy,xx,pos;
  
  for(yy=0; yy<height; yy++){
    for(xx=0; xx<width; xx++){
      pos = yy*width+xx;
      intoimage[pos] = fromimage[pos];
    }
  }
}

void
CopyIm_UShortToFloat(unsigned short *fromimage, float *intoimage, int height, int width)
{
  int yy,xx,pos;
  
  for(yy=0; yy<height; yy++){
    for(xx=0; xx<width; xx++){
      pos = yy*width+xx;
      intoimage[pos] = (float) fromimage[pos];
    }
  }
}

void
PadIm_UShortToFloat(unsigned short *fromimage, float *intoimage, int height, int width)
{
  int yy,xx,pos;
  
  for(yy=1; yy<(height-1); yy++){
    for(xx=1; xx<(width-1); xx++){
      pos = yy*width+xx;
      intoimage[pos] = (float) fromimage[(yy-1)*(width-2)+(xx-1)];
    }
  }
  adjust_boundaries_float(intoimage,height,width);
}

void
PadIm_UShortToDouble(unsigned short *fromimage, double *intoimage, int height, int width)
{
  int yy,xx,pos;
  
  for(yy=1; yy<(height-1); yy++){
    for(xx=1; xx<(width-1); xx++){
      pos = yy*width+xx;
      intoimage[pos] = (double) fromimage[(yy-1)*(width-2)+(xx-1)];
    }
  }
  adjust_boundaries_double(intoimage,height,width);
}

void
CopyZLevelToUByte(float *dimage,unsigned char *bimage,int height,int width)
{
  int yy,xx;
  int dvalue,pos;
  
  pos=0;
  for(yy=0; yy<height; yy++) {
    for(xx=0; xx<width; xx++) {
      dvalue = (int) (dimage[(yy+1)*(width+2)+(xx+1)]);
      if (dvalue == 0) {
         bimage[pos] = 0; 
      }
      else bimage[pos] = 255;
      pos++;
    }
  }
 }

void
CopyIm_FloatToFloat(float *fromimage, float *intoimage, int height, int width)
{
 int yy,xx,pos;
  
 pos =0;
 for(yy=0; yy<height; yy++){
   for(xx=0; xx<width; xx++){
     pos = yy*width+xx;
     intoimage[pos] = (float) fromimage[pos];
   }
 }
}

void
CopyIm_FloatToUChar(float *fromimage, unsigned char *intoimage, int height, int width)
{
  int yy,xx,pos;
  
  pos =0;
  for(yy=0; yy<height; yy++){
    for(xx=0; xx<width; xx++){
      intoimage[pos] = (unsigned char) fromimage[pos];
      pos++;
    }
  }
}

void
CopyIm_FloatToUShort(float *fromimage, unsigned short *intoimage, int height,int width)
{
  int yy,xx,pos;

  pos =0;
  for(yy = 0;yy < height; yy++){
    for(xx = 0;xx < width; xx++){
      intoimage[pos] = (unsigned short) fromimage[pos];
      pos++;
    }
  }
}

void
InitializeImage_Float (float *array, float value, int height, int width)
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

void
InitializeImage_Double(double *array, double value, int height, int width)
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

void
InitializeImage_UChar (unsigned char *array, unsigned char value, int height, int width)
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

float
min_of_5(float val1, float val2, float val3, float val4, float val5)
{
  float val;
  val = 100011.0;
  if (val1 < val) val = val1;
  if (val2 < val) val = val2;
  if (val3 < val) val = val3;
  if (val4 < val) val = val4;
  if (val5 < val) val = val5;
  return(val);
}
 
float
image_element_substitute (int x, int y, float *image,int height,int width)
{
  int xx,yy;

  if (x < 0)           xx = 0;
  else if (x < width)  xx = x;
  else                 xx = width - 1;

  if (y < 0)           yy = 0;
  else if (y < height) yy = y;
  else                 yy = height - 1;

  return(image[yy*width + xx]);  
}

double
image_element_substitute_double (int x, int y, double *image,int height,int width)
{
  int xx,yy;

  if (x < 0)           xx = 0;
  else if (x < width)  xx = x;
  else                 xx = width - 1;

  if (y < 0)           yy = 0;
  else if (y < height) yy = y;
  else                 yy = height - 1;

  return(image[yy*width + xx]);  
}

float
image_element_substitute_int (int x, int y, int *image,int height,int width)
{
  int xx,yy;

  if (x < 0)           xx = 0;
  else if (x < width)  xx = x;
  else                 xx = width - 1;

  if (y < 0)           yy = 0;
  else if (y < height) yy = y;
  else                 yy = height - 1;

  return(image[yy*width + xx]);  
}

void
adjust_boundaries_float(float *image, int height, int width) 
{
  int x,y,pos;

/* corners */
   image[0*width+0] = image[1*width+1];
   image[0*width+width-1] = image[1*width+(width-2)];
   image[(height-1)*width+0] = image[(height-2)*width+1];
   image[(height-1)*width+width-1] = image[(height-2)*width+(width-2)];
   
/* left and right side */
  for(y=0; y<height; y++) {
     image[y*width+0] =  image[y*width+1];
     image[y*width+(width-1)] =  image[y*width+(width-2)];
   }

/* top and bottom sides */ 
  for(x=0; x<width; x++) {
     image[0*width+x] =  image[1*width+x];
     image[(height-1)*width+x] =  image[(height-2)*width+x];
   }
}

void
adjust_boundaries_double(double *image, int height, int width) 
{
  int x,y,pos;

/* corners */
   image[0*width+0] = image[1*width+1];
   image[0*width+width-1] = image[1*width+(width-2)];
   image[(height-1)*width+0] = image[(height-2)*width+1];
   image[(height-1)*width+width-1] = image[(height-2)*width+(width-2)];
   
/* left and right side */
  for(y=0; y<height; y++) {
     image[y*width+0] =  image[y*width+1];
     image[y*width+(width-1)] =  image[y*width+(width-2)];
   }

/* top and bottom sides */ 
  for(x=0; x<width; x++) {
     image[0*width+x] =  image[1*width+x];
     image[(height-1)*width+x] =  image[(height-2)*width+x];
   }
}

void
adjust_boundaries_int(int *image, int height, int width) 
{
  int x,y,pos;

/* corners */
   image[0*width+0] = image[1*width+1];
   image[0*width+width-1] = image[1*width+(width-2)];
   image[(height-1)*width+0] = image[(height-2)*width+1];
   image[(height-1)*width+width-1] = image[(height-2)*width+(width-2)];
   
/* left and right side */
  for(y=0; y<height; y++) {
     image[y*width+0] =  image[y*width+1];
     image[y*width+(width-1)] =  image[y*width+(width-2)];
   }

/* top and bottom sides */ 
  for(x=0; x<width; x++) {
     image[0*width+x] =  image[1*width+x];
     image[(height-1)*width+x] =  image[(height-2)*width+x];
   }
}

void
adjust_boundaries_uchar(unsigned char *image, int height, int width) 
{
  int x,y,pos;

/* corners */
   image[0*width+0] = image[1*width+1];
   image[0*width+width-1] = image[1*width+(width-2)];
   image[(height-1)*width+0] = image[(height-2)*width+1];
   image[(height-1)*width+width-1] = image[(height-2)*width+(width-2)];
   
/* left and right side */
  for(y=0; y<height; y++) {
     image[y*width+0] =  image[y*width+1];
     image[y*width+(width-1)] =  image[y*width+(width-2)];
   }

/* top and bottom sides */ 
  for(x=0; x<width; x++) {
     image[0*width+x] =  image[1*width+x];
     image[(height-1)*width+x] =  image[(height-2)*width+x];
   }
}

#include <string.h>
#include <ctype.h>

int
keymatch (char *str, char *keyword,int minchars) 
{
  register int len;
  
  len = strlen( str );
  if ( len < minchars )
    return 0;
  while ( --len >= 0 )
    {
      register char c1, c2;
      
      c1 = *str++;
      c2 = *keyword++;
      if ( c2 == '\0' )
        return 0;
        if ( isupper( c1 ) )
          c1 = tolower( c1 );
      if ( isupper( c2 ) )
        c1 = tolower( c2 );
        if ( c1 != c2 )
          return 0;
    }
  return 1;
}
 
void
print_heap(Heap *heap, int flag, int N)
{
  int ii;
  double x,y;
  
  if (flag == 1) {
    for(ii=1; ii<=N; ii++) {
      x = heap->locx[heap->index[ii]];
      y = heap->locy[heap->index[ii]];
      printf("ii = %i and data =%f and index[ii] = %i, loc[ii] = %i, and y = %f and x = %f\n",
       ii,heap->data[heap->index[ii]],heap->index[ii], heap->loc[ii],y,x);
    }
    printf("\n");
  }
}

float
sign_function(float value)
{
  if (value > 0.0) 
    return (1.0);
  else if (value < 0.0) 
    return (-1.0);
  else return (0.0);
}

double
sqr(double value)
{
  return (value*value);
}

int
mod(int number, int base)
{
  float t1,t2,t3;
  
  if ((number >= 0) && (number < base)) 
   return (number);
  else if (fabs(number) == base)
   return(0);
  else if (number > base){
   t1 = (float) number/(float) base;
   t2 = (int) t1;
   t3 = (t1-t2);
   return((int)(base*t3 +0.5));
  }
  else if (number < 0) {
   return ((int) (base+number));
  }
}

void
image_extremas_float(float *image, float *max, float *min, int height, int width) 
{
  int y,x,pos;
  float mx,mn;
  mx = -999999.0;
  mn = 9999999.0;
 
  for(y=0; y<width; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      if (image[pos] > mx)
        mx = image[pos];
      if (image[pos] < mn) 
        mn = image[pos];
    }
  }

  *max = mx;
  *min = mn;
}

void
image_extremas_int(int *image, int *max, int *min, int height, int width) 
{
  int y,x,pos;
  int mx,mn;
  mx = -999999.0;
  mn = 9999999.0;
 
  for(y=0; y<width; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      if (image[pos] > mx)
        mx = image[pos];
      if (image[pos] < mn) 
        mn = image[pos];
    }
  }

  *max = mx;
  *min = mn;
}

void
image_extremas_UChar(unsigned char *image, int *max, int *min, int height, int width) 
{
  int y,x,pos;
  int mx,mn;
  mx = -999999.0;
  mn = 9999999.0;
 
  for(y=0; y<width; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      if (image[pos] > mx)
        mx = image[pos];
      if (image[pos] < mn) 
        mn = image[pos];
    }
  }

  *max = mx;
  *min = mn;
} 

void
image_scale_UChar(unsigned char *image, int height, int width) 
{
  int y,x,pos;
  int mx,mn;
  int max,min;
  float image_value,range;
  
  image_extremas_UChar(image, &max, &min, height, width);
  
  range = (float) max-min;
  
  for(y=0; y<width; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      image_value = (float) image[pos];
      image[pos] = (int) (255.0*(image_value-min)/range);
    }
  }
}

void
set_image_UChar(unsigned char *image, int new_value, int value,  int height, int width) 
{
  int y,x,pos;
  
  for(y=0; y<width; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      if (image[pos] == value)
        image[pos] = new_value;
    }
  }
}

void
decode_2d_location(int location, int *y, int *x, int height, int width)
{
  double yf;
  int yy, xx;

  yf = (double) location/ (double) width;
  
  yy = (int) yf;
  
  xx= location-yy*width;
  
  /* printf("location =%i and y =%i and x = %i\n", location, yy, xx); */
  
  *y = yy;
  *x = xx;
}

void
decode_3d_location(int location, int *y, int *x, int height, int width)
{
  double yf;
  int yy, xx;

  yf = (double) location/ (double) width;
  
  yy = (int) yf;
  
  xx= location-yy*width;
  
  /* printf("location =%i and y =%i and x = %i\n"); */
  
  *y = yy;
  *x = xx;
}

/*=======================================================*/

