/* eno_image.c  -- Perry A. Stoll Fri May 31 1996
 *
 * Time-stamp: <96/07/08 16:48:39 pas>
 */

#ifndef lint
static char *rcsid = "$Header: /vision/projects/cvsroot2svn/cvsroot/lemsvxlsrc/algo/contourtracing/eno_image.cpp,v 1.2 2004-10-15 16:37:28 mleotta Exp $";
#endif

#include "1d_eno_types.h"

double eno_NearZeroValue = 1.0e-8;
double eno_IntervalTolerance = 1.0e-8;

/************************************************************
  @routine@ enoimage_compute_interpolants

  @param@ eno_image - structure to hold ENO interpolants computed here.
  @param@ data - raw data to interpolate.

  @desc@ This routine computes the ENO polynomial interpolants for all
  horizontal and vertical gridlines.

  @return@ int - not used.
  @end@
***********************************************************/
int
enoimage_compute_interpolants(ENO_Interval_Image_t *eno_image,
            double const *data)

{
  int const h = eno_image->height;
  int const w = eno_image->width;
  dbg_enter("enoimage_compute_interpolants");

  /* compute eno interpolants for all horizontal intervals, i.e. x direction  */
  enointerv_compute_interpolants_along_all_gridlines(eno_image->horiz,data,h,w,w,1);
    
  /* compute eno interpolants for all vertical intervals, i.e. y direction  */
  enointerv_compute_interpolants_along_all_gridlines(eno_image->vert,data,w,1,h,w);

  dbg_enter("enoimage_compute_interpolants");

  return kENOSuccess;
}

/************************************************************
  @routine@ enoimage_compute_shocks

  @param@ eno_image - structure containing previously computed ENO
  interpolants and storage for shocks computed here.

  @desc@ This routine examines the ENO interpolants for every interval
  in the image and  computes and places ENO shocks where needed.

  @return@ int - not used.
  @end@
***********************************************************/
int
enoimage_compute_shocks(ENO_Interval_Image_t *eno_image)

{
  const int
    h = eno_image->height,
    w = eno_image->width;

  dbg_enter("enoimage_compute_shocks");
  
  enoimage_free_shocks(eno_image);
  
  /* compute all eno intervals for all horizontal intervals, i.e. x
     direction */
  enointerv_place_shocks_along_all_gridlines(eno_image->horiz,h,w,w,1);
    
  /* compute all eno intervals for all vertical intervals, i.e. y
     direction */
  enointerv_place_shocks_along_all_gridlines(eno_image->vert,w,1,h,w);

  dbg_leave("enoimage_compute_shocks");

  return kENOSuccess;
}


void enoimage_free_shocks(ENO_Interval_Image_t *image)
{
    int h,w;
    int xx,yy;

    h = image->height;
    w = image->width;

    for(yy = 0; yy < h; yy++){
        for(xx = 0; xx < w ; xx++){
            if ((image->horiz + yy * w + xx)->shock){
                enoDEL((image->horiz +yy * w + xx)->shock);
                (image->horiz +yy * w + xx)->shock = NULL;
            }
            if ((image->vert + yy * w + xx)->shock){
                enoDEL((image->vert +yy * w + xx)->shock);
                (image->vert +yy * w + xx)->shock = NULL;
            }
        }
    }
}

/************************************************************
  @routine@ enoimage_compute_zerox

  @param@ eno_image - structure containing previously computed ENO
  interpolants and storage for zerox computed here.

  @desc@ This routine examines the ENO interpolants for every interval
  in the image and  computes and places ENO zerox where needed.

  @return@ int - not used.
  @end@
***********************************************************/
int
enoimage_compute_zerox(ENO_Interval_Image_t *eno_image)

{
  const int
    h = eno_image->height,
    w = eno_image->width;

  dbg_enter("enoimage_compute_zerox");
  
  enoimage_free_zerox(eno_image);
  
  /* compute all eno intervals for all horizontal intervals, i.e. x
     direction */
  enointerv_place_zerox_along_all_gridlines(eno_image->horiz,h,w,w,1);
    
  /* compute all eno intervals for all vertical intervals, i.e. y
     direction */
  enointerv_place_zerox_along_all_gridlines(eno_image->vert,w,1,h,w);

  dbg_leave("enoimage_compute_zerox");

  return kENOSuccess;
}

void
enoimage_free_zerox(ENO_Interval_Image_t *image)
{
    int h,w;
    int xx,yy;

    h = image->height;
    w = image->width;

    for(yy = 0; yy < h; yy++){
        for(xx = 0; xx < w ; xx++){
            if (image->horiz[yy * w + xx].zerox != NULL){
                enoDEL(image->horiz[yy * w + xx].zerox);
                image->horiz[yy * w + xx].zerox = NULL;
            }
            if (image->vert[yy * w + xx].zerox != NULL){
                enoDEL(image->vert[yy * w + xx].zerox);
                image->vert[yy * w + xx].zerox = NULL;
            }
        }
    }
}

ENO_Interval_Image_t *
enoimage_from_data(double *surface,int height,int width)
{
    
    ENO_Interval_Image_t *enoimage = enoimage_new(NULL,height,width);

    if (enoimage != NULL){
        enoimage_compute_interpolants(enoimage,surface);
        enoimage_compute_shocks(enoimage);
        enoimage_compute_zerox(enoimage);
    }
    return enoimage;
}


ENO_Interval_Image_t *
enoimage_new(ENO_Interval_Image_t *enoimage,
                                   int height,int width)
{
    ENO_Interval_Image_t *return_image;

    return_image = (enoimage == NULL) ? enoNEW(ENO_Interval_Image_t,1) : enoimage;

    if (return_image == NULL) return NULL;
    
    if (enoimage_alloc_data(return_image,height,width) == kENOFailure){
        /* free storage for return image only if we allocated it in
           this routine */
        if (enoimage == NULL)
            free(return_image);
        return_image = NULL;
    }
    return return_image;
}

#include <string.h>
int
enoimage_alloc_data(ENO_Interval_Image_t *enoimage,int height,int width)
{
    enoimage->height = height;
    enoimage->width = width;

    enoimage->horiz = enoNEW(ENO_Interval_t,height*width);
    if (NULL == enoimage->horiz){
        return kENOFailure;
    }
    
    enoimage->vert = enoNEW(ENO_Interval_t,height*width);
    if (NULL == enoimage->vert){
        free(enoimage->horiz);
        return kENOFailure;
    }
    memset((char *)enoimage->horiz,0,height*width*sizeof(ENO_Interval_t));
    memset((char *)enoimage->vert, 0,height*width*sizeof(ENO_Interval_t));
    
    return kENOSuccess;
}


void
enoimage_dealloc_data(ENO_Interval_Image_t *enoimage)
{
    enoimage_free_shocks(enoimage);
    enoimage_free_zerox(enoimage);
    enoDEL(enoimage->horiz);
    enoDEL(enoimage->vert);
    
}


void
enoimage_free(ENO_Interval_Image_t *enoimage)
{
    enoimage_dealloc_data(enoimage);
    free(enoimage);
}


