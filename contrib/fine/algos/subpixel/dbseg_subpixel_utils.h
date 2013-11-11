#ifdef __cplusplus
extern "C" {
#endif

#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define PI 3.14159265358979310
#define HPI 1.57079632679489660


/***************** heap.h *************************/
#define LARGE_NUMBER 99999.0
#define SMALL_NUMBER -99999.0

typedef struct {
  int N;
  double *data;
  int *index, *loc;
  int *locx,*locy, *locz;
  int size;
}Heap;


void up_minheap(Heap *heap, int k);
void insert_to_minheap(Heap *heap, int location, double data);
void down_minheap(Heap *heap, int k); 
void remove_from_minheap(Heap *heap);


void up_maxheap(Heap *heap, int k);
void insert_to_maxheap(Heap *heap, int location, double data);
void down_maxheap(Heap *heap, int k); 
void remove_from_maxheap(Heap *heap);


/****************************pgm.h ****************************/
typedef struct {
  char *type;
  int h,w,maxval;
  unsigned char *im;
}PgmImage;


void read_pgmimage(char *file, PgmImage *pgmim);
void write_pgmimage(char *file, PgmImage *pgmim);
void InitPgm(PgmImage *pgmim); 

/************************** convolve.h **********************/

double gauss (double x, double stdev, double center);
void gaussian_kernel (double stdev, double range, double center, double *kernel, 
                      int *offset, double *kernel_sum);
void copy_image_and_record_extrema (double *image_in, double *image_out, 
                                    double *min, double *max, int width , int height);
void gauss_conv (double *image, double *into_image, double sigma_x, double sigma_y,
                 double range, double weight, int width, int height);                 
    
/********************* level.h ***************************/

void get_level_set (double *surface_array, double *level_set_array, double level, 
                    int boundary_value,  int interpolate, int band_size,int height, int width);
void find_zero_crossing(double *level_set_array, double *surface_array, 
                        double point_value, double level, int boundary_value, int y, int x, 
                        int interpolate, int height, int width);
void linear_interpolation(double *level_set_array, double interpolation,
                          int position, int new_position);  

/*********************** contour_routines.h  ***************************/

typedef struct vector
{
    short        dir;
    struct vector *    next;
} DiscreteVector;

struct boundsrect
{
    int x_min;
    int y_min;
    int x_max;
    int y_max;
};

typedef struct boundsrect Boundsrect;

/*** This lets you treat a one-d array as a 2-day array.  x_dim is the largest x
  index.  Thus, an 8X8 array would have x_dim as 7.    ***/

/*#define Get2D(the_array,x_dim,y,x) (the_array[y*(x_dim+1)+x])*/
#define CHAINEND -1
#define ENDOFCHAINLIST -2


typedef struct pre_chainmap
{
    int h_size;
    int v_size;
    int *map;
} chainmap;

/**********************
* The FUNCTIONS
**********************/
short in_cont(int x, int y, int elev, const int *sample, const Boundsrect *boundary);
short probe(int x, int y, int elev, int dir, int *new_x, int *new_y, 
        const int *sample, const Boundsrect *boundary);
int neighbor(int x, int y, int elev, int last_dir, int *new_x, int *new_y, 
         const int *sample, const Boundsrect *boundary);
void find_contour(int elev,int *chain,int chainlength,const int *sample,int width,int height,
          int start_x, int start_y,int *gnew,chainmap *contour_map);
void report(int elev, const int *chain);
void contour_tracer (int elevation,int *  chain,int chainlength, int *sample,
         int width,int height);
int dir2point (int x, int y, int dir, int *new_x, int *new_y);
int Get2D(const int* the_array, int x_dim, int y,int x);
int power (int base, int n);
int make_bitmap(int h_size,int v_size, chainmap **returned_chainmap);
void setpos (int h_pos,int v_pos,chainmap *change_map);
int getpos (int h_pos,int v_pos, chainmap *change_map);
void save_contour_points (char* filename,const int *chain);
              

/************************************basic_utils.h ***************************/


void CopyIm_UCharToFloat(unsigned char *fromimage, float *intoimage, int height, int width);
void CopyIm_FloatToDouble(float *fromimage, double *intoimage, int height, int width);
void CopyIm_DoubleToDouble(double *fromimage, double *intoimage, int height, int width);
void PadIm_UCharToFloat(unsigned char *fromimage, float *intoimage, int height, int width);
void PadIm_UCharToDouble(unsigned char *fromimage, double *intoimage, int height, int width);
void PadIm_UShortToDouble(unsigned short *fromimage, double *intoimage, int height, int width);
void CopyIm_UCharToInt(unsigned char *fromimage, int *intoimage, int height, int width);
void CopyIm_UCharToUChar(unsigned char *fromimage,unsigned char *intoimage,
             int height,int width);
void CopyIm_UShortToFloat(unsigned short *fromimage, float *intoimage, int height, int width);
void PadIm_UShortToFloat(unsigned short *fromimage, float *intoimage, int height, int width);
void PadIm_UCharToUChar(unsigned char *fromimage, unsigned char*intoimage, 
                        int height, int width);
void CopyZLevelToUByte(float *dimage,unsigned char *bimage,int height,int width);
void CopyIm_FloatToFloat(float *fromimage, float *intoimage, int height, int width);
void CopyIm_FloatToUChar(float *fromimage, unsigned char *intoimage, int height,int width);
void CopyIm_FloatToUShort(float *fromimage, unsigned short *intoimage, int height,int width);
void InitializeImage_Float (float *array, float value, int height, int width);
void InitializeImage_UChar (unsigned char *array, unsigned char value, int height, int width);
void InitializeImage_Double(double *array, double value, int height, int width);
float min_of_5(float val1, float val2, float val3, float val4, float val5);
float image_element_substitute (int x, int y, float *image,int height,int width);
float image_element_substitute_int (int x, int y, int *image,int height,int width);

double
image_element_substitute_double (int x, int y, double *image,int height,int width);
void adjust_boundaries_float(float *image, int height, int width); 
void adjust_boundaries_double(double *image, int height, int width); 
void adjust_boundaries_int(int *image, int height, int width); 
void adjust_boundaries_uchar(unsigned char *image, int height, int width); 
int  keymatch (char *str, char *keyword,int minchars); 
void print_heap(Heap *heap, int flag, int N);
float sign_function(float value);
//eli edit     double sqr(double value);
int mod(int number, int base);
void image_extremas_float(float *image, float *max, float *min, int height, int width);
void image_extremas_int(int *image, int *max, int *min, int height, int width);
void image_extremas_UChar(unsigned char *image, int *max, int *min, int height, int width);
void image_scale_UChar(unsigned char *image, int height, int width);
void set_image_UChar(unsigned char *image, int new_value, int value,  int height, int width);
void decode_2d_location(int location, int *y, int *x, int height, int width);
/************************* SimpleContour.h *******************************/

typedef struct SimpleContour {
  int* data;
  int length;
} SimpleContour_t;

SimpleContour_t* CreateSimpleContour(int);
void DestroySimpleContour(SimpleContour_t*);
SimpleContour_t* GetSimpleContour(FILE*);
void PrintSimpleContour(FILE* file,SimpleContour_t*);

/**************************************copy-utils.h*************************************/
void PadIm_UCharToIntP(unsigned char *fromimage,int *intoimage,
              int height,int width,int pad);
void PadIm_UCharToUCharP(unsigned char *fromimage,unsigned char *intoimage,
            int height,int width, int pad);

void PadIm_IntToIntP(int *fromimage,int *intoimage,
            int height,int width,int pad);

void PadIm_DoubleToDoubleP(double *fromimage, double *intoimage, 
              int height, int width,int pad);

void UnPadIm_DoubleToDouble(double *fromimage,double *intoimage, 
                int height, int width,int pad);

void UnPadIm_UCharToUChar(unsigned char *fromimage,unsigned char *intoimage, 
              int height, int width,int pad);
void UnPadIm_FloatToFloat(float *fromimage,float *intoimage, 
              int height, int width,int pad);
void UnPadIm_IntToInt(int *fromimage, int *intoimage, 
              int height, int width,int pad);
void CopyIm_DoubleToDouble(double *bimage,double *dimage,int height,int width);

void CopyIm_DoubleToFloat(double *bimage, float *dimage,int height,int width);

void CopyIntToInt(int *bimage, int *dimage,
              int depth,int height,int width);
void CopyUByteToFloat(unsigned char *bimage,float *dimage,int depth,int height,int width);
void CopyUByteToUByte(unsigned char *bimage, unsigned char *dimage,
              int depth,int height,int width);
void CopyUByteToInt(unsigned char *bimage, int *dimage,
              int depth,int height,int width);
void CopyUShortToFloat(unsigned short *bimage,float *dimage,int depth,int height,int width); 
void CopyUShortToInt(unsigned short *bimage,int *dimage,int depth,int height,int width);
void CopyFloatToUByte(float *dimage,unsigned char *bimage,int depth,int height,int width); 
void CopyFloatToFloat(float *fimage,float *dimage,int depth,int height,int width); 
void CopyFloatToUShort(float *dimage,unsigned short *uimage,int depth,int height,int width);
int pixelread(int x,int y, int width, int *contour_array);
void pixelwrite(int x,int y,int width, int nv,int *contour_array);

void CopyIntToUByte(int *bimage, unsigned char *dimage,
              int depth,int height,int width);

void print_2d_int_array(int *a, int h, int w, FILE *fp);

void print_2d_uchar_array(unsigned char *a, int h, int w, FILE *fp);

void print_2d_float_array(float *a, int h, int w, FILE *fp);

void adjust_boundaries_pad_float(float *image, int height, int width, int pad);

void adjust_boundaries_pad_int(int *image, int height, int width, int pad);

void adjust_boundaries_pad_double(double *image, int height, int width, int pad);
void adjust_boundaries_pad_uchar(unsigned char *image, int height, int width, int pad);

/* _copyutils_h_ */

/***************************************************************************************/



#endif /* UTILS */

#ifdef __cplusplus
}
#endif

