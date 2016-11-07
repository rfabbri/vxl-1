#ifdef __cplusplus
extern "C" {
#endif

#ifndef _3D_UTILS_H_
#define _3D_UTILS_H_

void DoubleToFloat(double *imagefrom,float *fimage, int dt, int ht, int wt);
void IntToFloat(int *imagefrom,float *fimage, int dt, int ht, int wt);
void UShortToFloat(unsigned short *imagefrom,float *fimage, int dt, int ht,
           int wt);
void UCharToFloat(unsigned char *imagefrom,float *fimage, int dt, int ht,
          int wt);
void CopyIm3d_UCharToDouble(unsigned char *pucFromImage, double *pdIntoImage, int iFullSize);
void CopyIm3d_FloatToDouble(float *pfFromImage, double *pdIntoImage, int iFullSize);
void initialize_uchar_image(unsigned char *array,int value, int depth, 
                int height,int width);
void initialize_uchar_2dimage(unsigned char *array, int value, int height,
                  int width);
void initialize_float_image(float *array, float value, int depth,int height,
                int width);
float image_element_substitute_3d (int x, int y, int z, float *image,
                   int depth, int height, int width);
int image_element_substitute_ubyte_3d (int x, int y, int z,
                       unsigned char *image,
                       int depth, int height, int width);
int image_element_substitute_ushort_3d (int x, int y,int z,
                    unsigned short *image,
                    int depth, int height, int width);
void copy_image_float(float *fromimage, float *intoimage, int depth, 
              int height, int width);

#endif /* _3D_UTILS_H_ */

#ifdef __cplusplus
}
#endif

