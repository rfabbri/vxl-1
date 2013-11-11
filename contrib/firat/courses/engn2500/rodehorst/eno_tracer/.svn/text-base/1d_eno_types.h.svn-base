/* geno_types.h	-- Perry A. Stoll
 *
 * Time-stamp: <96/07/08 16:48:45 pas>
 * $Header$
 * $Log$
 */

#ifndef _1DENO_TYPES_H_
#define _1DENO_TYPES_H_


#include <stdio.h>
#include <math.h>
#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ENOIMAGE_VERSION "ENOImage Library 1.2"

#define dbg_enter(x)  /* fprintf(stderr,(x)) */
#define dbg_leave(x)  /* fprintf(stderr,(x)) */
#define message(a) fprintf(stderr,(a))
#define message2(a,b) fprintf(stderr,(a),(b))
#define message3(a,b,c) fprintf(stderr,(a),(b),(c))
#define dmessage(a) 
#define dmessage2(a,b) 
#define dmessage3(a,b,c) 
#define fatal_error(x) fprintf(stderr,x); exit(-1)
  
#define kENOFailure 1
#define kENOSuccess 0
  
#define kENOMaxOrder 2
#define kENODataLength (kENOMaxOrder + 2)
#define kENOMaxZerox (kENOMaxOrder)

#define kZeroOrderIndex   0
#define kFirstOrderIndex  (kZeroOrderIndex+1)
#define kSecondOrderIndex (kFirstOrderIndex+1)
  
#define kRadiansPerDegree (M_PI/180.0)
#define kDegreesPerRadian (180.0/M_PI)


#define enoNEW(_t,_cnt) (_t *)calloc(_cnt,sizeof(_t))
#define enoDEL(_p)      free(_p)

extern double eno_NearZeroValue;
extern double eno_IntervalTolerance;

#include "twod_dpoint.h"
  
/** List of points **/
typedef struct{
  int len;
  TwoD_DPoint loc[kENOMaxZerox+1];
}  TwoD_DPoint_list;


/* a few forward typedefs because of the nesting of these structures.  */
typedef struct ENO_Params_S            ENO_Params_t;
typedef struct ENO_ZeroCrossing_S      ENO_ZeroCrossing_t;
typedef struct ENO_Polynomial_S        ENO_Polynomial_t;
typedef struct ENO_Interpolant_S       ENO_Interpolant_t;
typedef struct ENO_Interval_S          ENO_Interval_t;
typedef struct ENO_Shock_Interpolant_S ENO_Shock_Interpolant_t;
typedef struct GENO_Interpolant_S      GENO_Interpolant_t;
typedef struct ENO_Interval_Image_S    ENO_Interval_Image_t;


/**************************************************
  @struct@ ENO_Params

  @desc@ Stores all run time threshold parameters used by the system.
  NOTE: not currently used.

  @member@ tangent_threshold - 
  @member@ curvature_threshold - 

  @end@
  **/
struct ENO_Params_S{
  double tangent_threshold;
  double curvature_threshold;
};


/**************************************************
  @struct@ ENO_Polynomial

  @desc@ Structure describing the polynomial constructed by ENO.

  @member@ order - order of the polynomial. currently fixed at 2.
  @member@ coeff - coefficients of the polynomial.
  @end@
  **/
struct ENO_Polynomial_S{
    double coeff[kENOMaxOrder + 1];
    int order;
};



/**************************************************
  @struct@ ENO_ZeroCrossing

  @desc@ Stores information for zero crossings of the ENO polynomials
  withing a given interval.

  @member@ cnt - number of zero crossings.
  @member@ loc - location of zero crossings.
  @member@ slope - slope of the data at the given zero crossings.

  @end@
  **/
/** List of information for zero crossings within an interval **/
struct ENO_ZeroCrossing_S{
  int cnt;
  double loc[kENOMaxZerox+1];
  double slope[kENOMaxZerox+1];
};


/**************************************************
  @struct@ ENO_Interpolant
  
  @desc@ Stores information about each interpolating polynomial and
  which intervals contributs to it.

  @member@ eno_indices indicies of data used for computing the ENO
  polynomial.

  @member@ forward - boolean showing which direction (foward or
  backward) to include in the stencil for

  @member@ start_index,end_index - first,last index of data used for
  interpreting the ENO polynomial.

  @member@ poly - polynomial for this interpolation interval.
  NOTE: polynomial is not normalized to interval (0,1).
  
  @member@ start_tangent,end_tangent - tangent in degrees at
  beginning, end of interval.

  @member@ start_curvture,end_curvature - curvature at beginning, end
  of interval.

  @end@
  **/
struct ENO_Interpolant_S{
  double start,end;
  int forward;
  ENO_Polynomial_t poly;
  double start_tangent,end_tangent;
  double start_curvature,end_curvature;
  double total_curvature;
};


/**************************************************
  @struct@ ENO_Shock_Interpolant

  @desc@ Stores information about each interpolating polynomials in an
  interval where a shock is placed

  @member@ forward, backward - separate ENO interpolants for both
  sides of the shock

  @member@ shock_loc - location of the shock in the interval
  @end@
  **/
struct ENO_Shock_Interpolant_S
{
    ENO_Interpolant_t forward, backward;
    double shock_loc;
};



/**************************************************
  @struct@ ENO_Interval
  
  @member@ interp - polynomial interpolant based on ENO for this
  interval
  
  @member@ shock - if non-NULL, list of shocks computed within this
  interval.

  @member@ zerox - if non-NULL, list of zero crossings of the
  polynomial interpolants in this interval.

  @desc@ Stores information about each ENO Interval
  @end@
  **/
struct ENO_Interval_S{
    ENO_Interpolant_t interp;
    ENO_Shock_Interpolant_t *shock;
    ENO_ZeroCrossing_t *zerox;
};
 


/**************************************************
  @struct@ GENO_Interpolant_S
  
  @desc@ Stores information about each GENO Interval.

  @member@ geno_indicies - coefficients of GENO polynomial
  interpolation.

  @member@ center - a point defining the center of the interpolated
  circular arc.

  @member@ radius - radius of interpolanted circular arc.

  @member@ start_angle - staring angle of circular arc.

  @member@ end_angle - ending angle of circular arc.

  @member@ CW - boolean flag indicating direction of interpretation of
  angles, either clockwise if true or counterc-clockwise otherwise.

  @end@
  **/
struct GENO_Interpolant_S
{
    int geno_indices[kENOMaxOrder + 1];
    double center_x, center_y, radius;
    double start_angle, stop_angle;
    int CW;
};



/**************************************************
  @struct@ ENO_Interval_Image
  
  @desc@ Stores ENO polynomial interpolants for both 'x' and 'y'
  directions along all gridlines in the two dimensional surface.

  For i in [0,width) and  j [0,height),
     X is an array of length [height*width] intervals,
       and stores the ENO_Interval computed for each interval (i -> i+1,j)
     Y is an array of length [height*width] intervals,
       and stores the ENO_Interval computed for each interval (i,j ->j+1)
  
  @member@ height - number of gridlines in vertical direction. 

  @member@ width - number of gridlines in horizontal direction. 
  
  @member@ x - ENO_Intervals along x or horizontal gridlines.

  @member@ y - ENO_Intervals along y or vertical gridlines.
  @end@
**/
struct ENO_Interval_Image_S
{ 
    int height, width;
    ENO_Interval_t *horiz, *vert;
};


double angle_difference(double angle1,double angle2,int degrees_p);
int is_almost_zero(double val);
int is_almost_equal(double val1,double val2, double tolerance);
int is_in_interval(double p,double start,double end);

/* some macros, trading safety for speed */
#define IS_ALMOST_ZERO(_v)            (fabs((double)(_v)) < eno_NearZeroValue)
#define IS_ALMOST_EQUAL(_v1,_v2,_tol) (fabs((double)((_v1) - (_v2))) < (_tol))
#define IS_IN_INTERVAL(_p,_s,_e)      ((_p) >= (_s) && (_p) <= (_e))


#define enointerp_copy(_dest,_src) \
   do{memcpy((_dest),(_src),sizeof(ENO_Interpolant_t));}while(0)

extern double enoshock_tangent_change_threshold;
extern double enoshock_curvature_change_threshold;

/* eno_image.c */
int enoimage_compute_interpolants(ENO_Interval_Image_t *eno_image,
				  double const *data);
int enoimage_compute_shocks(ENO_Interval_Image_t *eno_image);
int enoimage_compute_zerox(ENO_Interval_Image_t *eno_image);

void enoimage_free_shocks(ENO_Interval_Image_t *eno_image);
void enoimage_free_zerox(ENO_Interval_Image_t *eno_image);

ENO_Interval_Image_t *enoimage_new(ENO_Interval_Image_t *enoimage,
                                   int height,int width);
ENO_Interval_Image_t *enoimage_from_data(double *surface,
                                         int height,int width);
void enoimage_dealloc_data(ENO_Interval_Image_t *enoimage);
void enoimage_free(ENO_Interval_Image_t *enoimage);


/* eno_interval.c */
int enointerv_compute_interpolants_along_gridline(ENO_Interval_t *interval,
                                                  double const *data,
                                                  int len, int stride);

int enointerv_compute_interpolants_along_all_gridlines(ENO_Interval_t *interval,
                                                       double const *data,
                                                       int gl_len,int gl_stride,
                                                       int len,int stride);

int enointerv_place_shocks_along_gridline(ENO_Interval_t *interval,
                                          int len, register int stride);

int enointerv_place_shocks_along_all_gridlines(ENO_Interval_t *interval,
                                               int gl_len,int gl_stride,
                                               int len, int stride);

int enointerv_place_zerox_along_gridline(ENO_Interval_t *interval,
                                          int len, register int stride);

int enointerv_place_zerox_along_all_gridlines(ENO_Interval_t *interval,
                                               int gl_len,int gl_stride,
                                               int len, int stride);

int enointerv_print(ENO_Interval_t *interval, FILE *fp);


/* eno_interpolant.c */

double enointerp_derivative_at(ENO_Interpolant_t const *interp,
                               double loc);

int enointerp_compute_polynomial(ENO_Interpolant_t *interp,
                                 double x_loc,
                                 double const data[kENODataLength]);

int enointerp_compute_geometric_values(ENO_Interpolant_t *interp);


int enointerp_make_interpolant(ENO_Interpolant_t *interp,
                               double start,double end,
                               double const data[kENODataLength]);

int enointerp_print(ENO_Interpolant_t *interp,FILE *fp);


/* eno_shock.c */

int enoshock_make_shock(ENO_Interval_t *prev,ENO_Interval_t *cur,
                        ENO_Interval_t *next);

int enoshock_check_for_shock(ENO_Interval_t *prev,ENO_Interval_t *cur,
                             ENO_Interval_t *next);

int enoshock_print(ENO_Shock_Interpolant_t *shock, FILE *fp);

/* eno_zero_crossing.c */
int enozerox_from_polynomial(ENO_ZeroCrossing_t *zerox,
                             ENO_Polynomial_t const* poly);
int enozerox_from_polynomial_in_interval(ENO_ZeroCrossing_t *zeros,
                                         ENO_Polynomial_t *poly,
                                         double start, double end);
int enozerox_check_for_zerox_in_interval(ENO_Interval_t *prev,
                                         ENO_Interval_t *cur,
                                         ENO_Interval_t *next);

#define enozerox_copy(_d,_s) \
    do{memcpy((_d),(_s),sizeof(ENO_ZeroCrossing_t));}while(0)


int enozerox_print(ENO_ZeroCrossing_t const *zerox,FILE *fp);



#ifdef __cplusplus
}
#endif
#endif  /* _1DENO_TYPES_H_ */
