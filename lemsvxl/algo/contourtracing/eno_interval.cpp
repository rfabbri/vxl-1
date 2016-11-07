/* eno_interval.c  -- Perry A. Stoll Wed May 22 1996
 *
 * Time-stamp: <96/07/08 15:11:59 pas>
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2003/11/12 22:01:36  mcchang
 * MC..finished. it compiles
 *
 * Revision 1.1  2003/10/29 15:32:50  mcchang
 * Ming
 *
 * Revision 1.1.1.1  2003/03/31 18:17:17  mcchang
 * no message
 *
 */

#ifndef lint
static char *rcsid = "$Header: /vision/projects/cvsroot2svn/cvsroot/lemsvxlsrc/algo/contourtracing/eno_interval.cpp,v 1.3 2004-10-15 16:37:28 mleotta Exp $";
#endif

#include <stdio.h>
#include <vcl_cmath.h>
#include <limits.h>

#include "1d_eno_types.h"

/**
  @constant@ kBorderValue
  @desc@ value used to force ENO interpolantion to use data from other
  direction.
  @end@ 
  **/

//!!!!!!
#ifndef DBL_MAX
#define  DBL_MAX      1.7976931348623157E+308 /* max value of a "double"  */
#endif  

static const double kBorderValue = DBL_MAX/1.0e10;



/************************************************************
  @routine@ enointerv_compute_interpolants_along_gridline
  
  @param@ interval - One dimensional array of ENO intervals for where
  results are stored.
  @param@ data - surface data to interpolate.
  @param@ len - number of points in this one-dim gridline.
  @param@ stride - bytes to skip between successive entries the gridline.

  @desc@ This routine computes the ENO polynomial interpolants for a
  one dimensional set of data. 
  
  @return@ int - 0 for no error, 1 otherwise.
  @end@
  ************************************************************/
int
enointerv_compute_interpolants_along_gridline(ENO_Interval_t *interval,
                                              double const *data,
                                              int const len, int const stride)
{
    int
        ii;

    double
        neighborhood[kENODataLength]; /* storage for copying
                                                 local neighborhood */

    dbg_enter("enointerv_compute_interpolants_along_gridline");
  
    /* if don't have enough data to interpolate intervals, nothing to
       do here */
    if (len < kENODataLength){
        message("data gridline too short to interpolate.\n");
        dbg_leave("enointerv_compute_interpolants_along_gridline");
        return kENOFailure;
    }
  
    /* intialize loop index before calculations */
    ii = 0;

    /* A macro for improved readability */
#define ENOStoreDataList4(_l,_f0,_f1,_f2,_f3) \
    do{(_l)[0] = (_f0); (_l)[1] = (_f1); (_l)[2] = (_f2); (_l)[3] = (_f3);}while(0)
    
    
    /** do the first border value by hand **/
    ENOStoreDataList4(neighborhood,
                      kBorderValue,data[stride*0],
                      data[stride*1],data[stride*2]);
    enointerp_make_interpolant(&interval->interp,ii,ii+1,neighborhood);
    data     += stride;
    interval += stride;
  
    for(ii=1; ii < len-2; ii++){

        ENOStoreDataList4(neighborhood,
                          data[stride*-1],data[stride*0],
                          data[stride*1],data[stride*2]);
        enointerp_make_interpolant(&interval->interp,ii,ii+1,neighborhood);
    
        /* increment to next element */
        data     += stride;
        interval += stride;
    }
  
    /** do the last border value by hand **/
    ENOStoreDataList4(neighborhood,
                      data[stride*-1],data[stride*0],
                      data[stride*1],kBorderValue);
    enointerp_make_interpolant(&interval->interp,ii,ii+1,neighborhood);
    data     += stride;
    interval += stride;
  
#undef ENOStoreDataList4
  
    dbg_leave("enointerv_compute_interpolants_along_gridline");
    return kENOSuccess;
}

/************************************************************
  @routine@ enointerv_compute_interpolants_along_all_gridlines

  @param@ interval - structure to hold ENO representation computed here
  @param@ data - raw data that we will be interpolating
  @param@ cnt - number of gridlines in the data
  @param@ len - number of entries in one gridline
  @param@ stride - number of bytes between successive elements in a
  gridline

  @desc@ Compute the ENO polynomial interpolants for all intervals of
  the data in one direction.

  Note: having a stride paramter allows this routine to work for both
  horizontal and vertical passes through the data.

  @return@ int - not used.
  @end@
************************************************************/
int
enointerv_compute_interpolants_along_all_gridlines(ENO_Interval_t *interval,
                                                   double const *data,
                                                   int gl_len, int gl_stride,
                                                   int len, int stride)
{
  int ii;

  dbg_enter("enointerv_compute_interpolants_along_all_gridlines");

  if (len < kENODataLength){
      message("expected a padded image - length too short to interpolate");
  }else{

    for(ii = 0; ii < gl_len; ii++){
  
      /* compute the 1d eno interpolants for the current gridline */
        enointerv_compute_interpolants_along_gridline(interval,data,len,stride);

      /* increment pointers to next entry */
      interval += gl_stride;
      data     += gl_stride;
    }
  }
  dbg_leave("enointerv_compute_interpolants_along_all_gridlines");
  return kENOSuccess;
}


 
/************************************************************
  @routine@ enointerv_place_shocks_along_gridline

  @param@ len - number of points in this one-dim gridline
  @param@ stride - bytes to skip between successive elements
  @param@ interval - one dimensional array of ENO intervals where this
  routine stores its results.

  @desc@ This routine checks each interval along a given grid line to
  see if a shock should be placed in any interval. If a shock is
  needed, a new shock structure is allocated and stored in the
  appropriate interval.
  NOTE: we never place shocks at boundaries

  @return@ int - 0 for no error, 1 otherwise.
  @end@
************************************************************/
int
enointerv_place_shocks_along_gridline(ENO_Interval_t *interval,
                                      int len, int stride)
{
  int
    ii;
    
  dbg_enter("enointerv_place_shocks_along_gridline");
  interval += stride;
  for(ii=1; ii < len-2; ii++){
      enoshock_check_for_shock(interval - stride,interval,interval + stride);
      interval += stride;
  }
  
  dbg_leave("enointerv_place_shocks_along_gridline");
  return kENOSuccess;
}

/************************************************************
  @routine@ enointerv_place_shocks_along_all_gridlines

  @param@ interval - structure to hold ENO representation computed here
  @param@ gl_len - number of gridlines of data
  @param@ gl_stride - bytes between successive gridlines
  @param@ len - number of entries in one gridline
  @param@ stride - bytes between successive elements in a gridline

  @desc@ This routine checks to see if shocks need to placed in any
  interval along all gridlines in a given direction.

  Note: having a stride paramter allows this routine to work for both
  horizontal and vertical passes through the data.

  @return@ int - not used.
  @end@
************************************************************/
int
enointerv_place_shocks_along_all_gridlines(ENO_Interval_t *interval,
                                           int gl_len, int gl_stride,
                                           int len, int stride)
{
  int ii;

  dbg_enter("enointerv_place_shocks_along_all_gridlines");

  for(ii = 0; ii < gl_len; ii++){
      
    /* compute the 1d eno interpolants for the current gridline */
      enointerv_place_shocks_along_gridline(interval,len,stride);

    /* update gridline pointer to current gridline */
    interval += gl_stride;
  }

  dbg_leave("enointerv_place_shocks_along_all_gridlines");
  return kENOSuccess;
}




int
enointerv_place_zerox_along_gridline(ENO_Interval_t *interval,
                                      int len, int stride)
{
  int
      ii,
      n_zeros;
  
  dbg_enter("enointerv_place_shocks_along_gridline");
  interval += stride;
  for(ii=1; ii < len-2; ii++){

      n_zeros = enozerox_check_for_zerox_in_interval(interval-stride,
                                                     interval,interval+stride);
      interval+=stride;
  }
  
  dbg_leave("enointerv_place_shocks_along_gridline");
  return kENOSuccess;
}


int
enointerv_place_zerox_along_all_gridlines(ENO_Interval_t *interval,
                                           int gl_len, int gl_stride,
                                           int len, int stride)
{
  int ii;

  dbg_enter("enointerv_place_zerox_along_all_gridlines");

  for(ii = 0; ii < gl_len; ii++){
      
    /* compute the 1d eno interpolants for the current gridline */
      enointerv_place_zerox_along_gridline(interval,len,stride);

    /* update gridline pointer to current gridline */
    interval += gl_stride;
  }

  dbg_leave("enointerv_place_zerox_along_all_gridlines");
  return kENOSuccess;
}


int
enointerv_print(ENO_Interval_t *interval, FILE *fp)
{
  /*!!!!!enointerp_print(&interval->interp,fp);
  if (interval->shock){
    fprintf(fp,"  shock at ");
    enoshock_print(interval->shock,fp);
  }
  if (interval->zerox){
    fprintf(fp,"  zero crossings = ");
    enozerox_print(interval->zerox,fp);
  }*/
  return kENOSuccess;
}


