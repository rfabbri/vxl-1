#ifdef __cplusplus
extern "C" {
#endif

#ifndef __IMGUTIL_H
#define __IMGUTIL_H

void yuv2rgb(unsigned char *img,int size);
void rgb2yuv(unsigned char *img,int size);

void rgb2luv(unsigned char *RGB,float *LUV,int size);
void luv2rgb(unsigned char *RGB,float *LUV,int size);

void extendbounduc(unsigned char *img,unsigned char *imgout,int ny,int nx,int offset,int dim);
void extendboundfloat(float *img,float *imgout,int ny,int nx,int offset,int dim);


#endif

#ifdef __cplusplus
}
#endif

