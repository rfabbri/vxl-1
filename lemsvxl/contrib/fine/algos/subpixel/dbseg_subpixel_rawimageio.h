#ifdef __cplusplus
extern "C" {
#endif


/* Header file in /vision/projects_tek/c-utils/3d-utils/rawimageio.h  */


#ifndef _RAWIMAGEIO_H_
#define _RAWIMAGEIO_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <utils.h>

#include "3dUtils.h"

typedef struct {
  int d,h,w;
}Size;

typedef struct {
 int z,y,x,d,h,w;
}Crop;

typedef struct {
 int z,y,x;
}Pad;

typedef struct {
  unsigned char    *Uchar;
  unsigned short    *Ushort;
  int        *Int;
  float        *Float;
  double        *Double;
}ImageArrays;

typedef struct {
  char    type[10];
  char    jnk[128];
  Size    size;
  Crop    crop;
  Pad    pad;
  ImageArrays data;
}RawImage;


void
write_raw_image(char *file, RawImage *rawim);
void
Write3dRawImage(char *pcFileName, RawImage *pTheRawIm);
void
read_raw_image(char *file, RawImage *rawim);
void
Read3dRawImage(char *pcFileName, RawImage *pTheRawIm);
void
InitRawImage(RawImage *rawim);
 
#endif /* _RAWIMAGEIO_H_ */

#ifdef __cplusplus
}
#endif

