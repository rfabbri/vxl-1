#ifdef __cplusplus
extern "C" {
#endif

#ifndef _utils_h_
#define _utils_h_

void CopyUByteToUByte(unsigned char *bimage,unsigned char *dimage,
              int depth,int height,int width);
void CopyUByteToInt(unsigned char *bimage,int *dimage,
              int depth,int height,int width);
void CopyIntToUByte(int *bimage,unsigned char *dimage,
              int depth,int height,int width);
void CopyUByteToDouble(unsigned char *bimage,double *dimage,int depth,int height,int width);
void CopyUByteToFloat(unsigned char *bimage,float *dimage,int depth,int height,int width);
void CopyUShortToDouble(unsigned short *bimage,double *dimage,int depth,int height,int width);
void CopyUShortToFloat(unsigned short *bimage,float *dimage,int depth,int height,int width);
void CopyDoubleToUByte(double *dimage,unsigned char *bimage,int depth,int height,int width); 
void CopyDoubleToFloat(double *dimage,float *fimage,int depth,int height,int width); 
void CopyFloatToDouble(float *fimage,double *dimage,int depth,int height,int width); 
void CopyDoubleToUShort(double *dimage,unsigned short *uimage,int depth,int height,int width);
void initialize_double_image (double *array, double value, int height, int width); 
void initial_buble (double *scrap_array, int center_x, int center_y, int size,
                    double fill_in, int height, int width);
void copy_image (double *surface_array, double *aux_surface_array,
         int depth,int height,int width);
double find_norm (double *surface_array, double *aux_surface_array, int height, int width);
void initialize_bubbles (double *deviation_array, double *image_array, double thresh,
                         int height, int width);
void good_bubbles (double *image, double *speed_array, double thresh,
                   int height, int width);
double min(double val1, double val2, double val3, double val4, double val5);
void make_slice (double *array_3d, double *array_2d, int slice_direction, int slice,
                 int depth, int height, int width); 
double image_element_substitute (int x, int y, double *image,int height,int width);
int pixelread(int x,int y, int width, unsigned char *contour_array);
void pixelwrite(int x,int y,int width, int nv,unsigned char *contour_array);
int invert_image(int width, int height, unsigned char *contour_array);

#endif /* _copyutils_h_ */

#ifdef __cplusplus
}
#endif

