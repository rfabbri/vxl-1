#ifdef __cplusplus
extern "C" {
#endif

#include "dbseg_subpixel_subpixel_bubbles.h"

/********************************************************************
  Decsription:  See if a point is a valid one or not.
  Inputs : point - the point to be checked.
  Return : 1 if valid
           0 otherwise 
********************************************************************/
int is_valid_point(Point pt)
{
  if(pt.x == INVALID_POINT || pt.y == INVALID_POINT)
    return 0;
  else
    return 1;
}
 
/********************************************************************
  Decsription:  See if a label is a valid one or not.
  Inputs : label - the label to be checked.
  Return : 1 if valid
           0 otherwise 
********************************************************************/
int is_valid_label(int label)
{
  if(label == NOLABEL)
    return 0;
  else if (label > MAX_SHOCK_LENGTH)
    return 0;
  else
    return 1;
}

/********************************************************************
  Decsription:  Convert a surface to signed. Also takes sqrt of 
                distance (if flag is set).
  Inputs : surface 
           seed - 1 -> object
              0-> background
       height,width - image dimensions
       sqr_flag - 1-> take square root
                  0-> don't take sqrt
  Output : surface
********************************************************************/

void make_signed_distance_surface(double *surface,unsigned char *seed,
                  int height,int width, int sqr_flag)
{
  int i;
  for (i=0;i<height*width;i++){
    if (seed[i] == 1){
      if (sqr_flag)
    surface[i]=sqrt(fabs(surface[i]));
      else
    surface[i]=fabs(surface[i]);
    }
    else{
      if (sqr_flag)
    surface[i]=-1*sqrt(fabs(surface[i]));
      else
    surface[i]=-1*fabs(surface[i]);
    }
  }
}

#ifdef __cplusplus
}
#endif





