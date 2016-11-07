/* eno_label_zerox.c  -- Thomas Fri Nov 11 1997
 *
 * Time-stamp: <97/11/11 16:48:39 tbs>
 */

#ifndef lint
static char *rcsid = "$Header: /vision/projects/cvsroot2svn/cvsroot/lemsvxlsrc/algo/contourtracing/eno_label_zerox.cpp,v 1.3 2005-10-29 20:57:55 rfabbri Exp $";
#endif

#include "1d_eno_types.h"
#include "eno_label_zerox.h"


void eno_label_zerox(ENO_Interval_Image_t *eno_image,
         double *data, int *label)
{
  int h,w;
  int x,y;

  h = eno_image->height;
  w = eno_image->width;
  
  eno_label_zerox_along_all_gridlines(eno_image->vert,data,label,h,w,w); 

  eno_label_zerox_along_all_gridlines(eno_image->horiz,data,label,h,w,1); 

  
}

void eno_label_zerox_along_all_gridlines(ENO_Interval_t *interval,
           double *data, int *label,
           int height, int width, int stride)
{
  int x,y,pos;

  for (y=0;y<height-1;y++){
    for (x=0;x<width-1;x++){
      pos=y*width+x;

      if (interval[pos].zerox) {
        if (interval[pos].zerox->cnt == 1)
          eno_label_one_zerox_in_interval(&interval[pos],data,label,pos,stride);
        else if (interval[pos].zerox->cnt == 2)
          eno_label_two_zerox_in_interval(&interval[pos],data,label,pos,stride);
        else
          printf("This should not be happening No. of Zerox=%d\n",
           interval[pos].zerox->cnt);
      }

    }
  }
}

void eno_label_one_zerox_in_interval(ENO_Interval_t *interval, double *data,
             int *label, int pos, int stride)
{
  double start,end;

  start=data[pos];
  end=data[pos+stride];

  if (start > 0 && end > 0){
    /*printf("Both end points can't be +ve with one zerox\n");
    printf("Loc=%d, Str=%d\n",pos,stride);*/
    interval->zerox->label[0]=UNLABELLED;
  }
  else if (start > 0){
    interval->zerox->label[0]=label[pos];
  }
  else if (end > 0){
    interval->zerox->label[0]=label[pos+stride];
  }
  else{
    /*printf("Both end points can't be -ve with one zerox\n");
    printf("Loc=%d, Str=%d\n",pos,stride);*/
    interval->zerox->label[0]=UNLABELLED;
  }
}
  


void eno_label_two_zerox_in_interval(ENO_Interval_t *interval, double *data,
             int *label, int pos, int stride)
{

  double start,end;
  double start_label,end_label;
  double zerox1,zerox2;

  start=data[pos];
  end=data[pos+stride];
  start_label=label[pos];
  end_label=label[pos+stride];

  /*If both end points are +ve  label the zerox based on the closest
   end point*/
  if (start >= 0 && end >= 0){
    if (interval->zerox->loc[0] < interval->zerox->loc[1]){
      interval->zerox->label[0]=label[pos];
      interval->zerox->label[1]=label[pos+stride];
    }
    else{
      interval->zerox->label[1]=label[pos];
      interval->zerox->label[0]=label[pos+stride];
    }
  }
  /*If any of the end point is -ve cannot label the zerox*/
  else if (start < 0 || end < 0){
    // error
    interval->zerox->label[0]=UNLABELLED;
    interval->zerox->label[1]=UNLABELLED;
  }
}
