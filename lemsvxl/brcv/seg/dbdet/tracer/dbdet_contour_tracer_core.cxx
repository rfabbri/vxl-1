//:
// \file
// \brief Part of tracer which is heavily based on a historic C subpixel tracer code
//
// Check out lemsvxlsrc/algo/contourtracer for the complete OLD code in a less
// altered state.
//


#define dbdet_contour_tracer_internal_code 1
#include "dbdet_contour_tracer_core.h"

static int mod(int number, int base);
static void subpixel_contour_tracer(
    Tracer *Tr, CurrPt  *start_pt, Xings *xings, double label,
    int height, int width);
//static void merge_same_contours(Tracer *tracer, Tracer *tracer_final);

// The following are defined in dbdet_contour_tracer_core2.cxx
int find_initial_tracer_direction(Tr_Point curr, Tr_Point prev, int *sign);


void 
subpixel_contour_tracer_all(Tracer *Tr, Xings *xings, int height, int width)
{
  double *contour_array;
  int y,x,pos,ii;
  double label;
//  Tracer tracer_temp;
  CurrPt start_pt;

  contour_array = (double *) calloc(3*height*width,sizeof(double));

  ii=0;
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      if (xings->vert[pos].loc[0] >=0) {
        contour_array[ii] = xings->vert[pos].loc[0];
        contour_array[ii+1] = (double) x;
        contour_array[ii+2] = 1.0;
        ii+=3;
      }
      if (xings->vert[pos].loc[1] >=0) {
        contour_array[ii] = xings->vert[pos].loc[1];
        contour_array[ii+1] = (double) x;
        contour_array[ii+2] = 1.0;
        ii+=3;
      }
      if (xings->horiz[pos].loc[0] >=0) {
        contour_array[ii] = (double) y;
        contour_array[ii+1] = (double) xings->horiz[pos].loc[0];
        contour_array[ii+2] = 1.0;
        ii+=3;
      }
      if (xings->horiz[pos].loc[1] >=0) {
        contour_array[ii] = (double) y;
        contour_array[ii+1] = (double) xings->horiz[pos].loc[1];
        contour_array[ii+2] = 1.0;
        ii+=3;
      }
    }
  }

  contour_array[ii]   = -1.0;
  contour_array[ii+1] = -1.0;
  contour_array[ii+2] = -1.0;

  /*First pass with strict starting point restrictions*/
  ii =0; label=1.0; Tr->size=0;
  while(contour_array[ii] >= 0.0) {
    start_pt.y = contour_array[ii];
    start_pt.x = contour_array[ii+1];
    pos = (int)start_pt.y*width+ (int) start_pt.x;
    if ((xings->vert[pos].loc[0] >= 0.0) || (xings->horiz[pos].loc[0] >= 0.0) ||
        (xings->vert[pos].loc[1] >= 0.0) || (xings->horiz[pos].loc[1] >= 0.0)) {
      if (xings->vert[pos].cnt != 2 && xings->horiz[pos].cnt != 2 &&
        xings->vert[pos].label[0] != UNLABELLED &&
        xings->horiz[pos].label[0] != UNLABELLED &&
        xings->vert[pos].label[0] != UNLABELLED &&
        xings->horiz[pos].label[0] != UNLABELLED){

            subpixel_contour_tracer(Tr, &start_pt, xings,label,height,width);
            subpixel_contour_tracer(Tr, &start_pt, xings,label,height,width);
            label++;
      }
    }
    ii +=3;
  }

  ii =0; label=1.0;
  while(contour_array[ii] >= 0.0) {
    start_pt.y = contour_array[ii];
    start_pt.x = contour_array[ii+1];
    /////if (tracer_debug)
    /////  printf("starting point: %f and %f\n", start_pt.y,start_pt.x);
    pos = (int)start_pt.y*width+ (int) start_pt.x;
    if ((xings->vert[pos].loc[0] >= 0.0) || (xings->horiz[pos].loc[0] >= 0.0) ||
        (xings->vert[pos].loc[1] >= 0.0) || (xings->horiz[pos].loc[1] >= 0.0)) {
          subpixel_contour_tracer(Tr, &start_pt, xings,label,height,width);
          subpixel_contour_tracer(Tr, &start_pt, xings,label,height,width);
          label++;
    }
    ii +=3;
  }
  free(contour_array);

  Tr->horiz[Tr->size] = ENDOFLIST;
  Tr->vert[Tr->size] = ENDOFLIST; Tr->size++;
  Tr->horiz[Tr->size] = ENDOFLIST;
  Tr->vert[Tr->size] = ENDOFLIST;

#if 0    //  The following is Bogus code: If necessary, rewrite "merge_same_contours" in my own datastructure
         // - Ric. Fabbri Oct 2005.

  /* allocate memeory for tracer */
  tracer_temp.vert = (double *) calloc(Tr->size+5,sizeof(double));
  tracer_temp.horiz = (double *) calloc(Tr->size+5,sizeof(double));
//  tracer_temp.label = (int *) calloc(Tr->size+5,sizeof(int));

  merge_same_contours(Tr, &tracer_temp);
  ii=0;
  while (tracer_temp.vert[ii] != ENDOFLIST) {
    Tr->vert[ii] = tracer_temp.vert[ii];
    Tr->horiz[ii] = tracer_temp.horiz[ii];
//    Tr->label[ii] = tracer_temp.label[ii];
    ii++;
  }
  Tr->size = ii;
  Tr->horiz[Tr->size] = ENDOFLIST;
//  Tr->label[Tr->size] = ENDOFLIST;
  Tr->vert[Tr->size] = ENDOFLIST; Tr->size++;

//  Tr->label[Tr->size] = ENDOFLIST;
  Tr->horiz[Tr->size] = ENDOFLIST;
  Tr->vert[Tr->size] = ENDOFLIST;

  free(tracer_temp.vert);
  free(tracer_temp.horiz);
//  free(tracer_temp.label);
#endif

}

void 
subpixel_contour_tracer(
    Tracer *Tr, CurrPt  *start_pt, Xings *xings, double label,
    int height, int width)

{
  int s_dir,i,dir,temp_pos;
  int contour_length;
  int y,x,position,dir_sign,next_dir;
  CurrPt curr_pt, next_pt,end_pt;
  Tr_Point curr,prev;


  y = (int) start_pt->y;
  x = (int) start_pt->x;
  position = y*width+x;

  curr_pt.x = start_pt->x;
  curr_pt.y = start_pt->y;
  curr.x=curr_pt.x;
  curr.y=curr_pt.y;
  end_pt.type = NOXING; // added this to fix bug, but not sure if right

  if (tr_xing_type(&curr) == TR_VERT){
    label = (double)xings->vert[position].label[0];
    xings->vert[position].loc[0] = UNLABELLED;
    xings->vert[position].loc[1] = UNLABELLED;
  }
  else if (tr_xing_type(&curr) == TR_HORIZ){
    label = (double)xings->horiz[position].label[0];
    xings->horiz[position].loc[0] = UNLABELLED;
    xings->horiz[position].loc[1] = UNLABELLED;
  }
  else if (tr_xing_type(&curr) == TR_GRID){
    if (xings->vert[position].loc[0] >= 0.0)
      label=(double)xings->vert[position].label[0];
    else
      label=(double)xings->horiz[position].label[0];
    xings->vert[position].loc[0]  = UNLABELLED;
    xings->vert[position].loc[1]  = UNLABELLED;
    xings->horiz[position].loc[0] = UNLABELLED;
    xings->horiz[position].loc[1] = UNLABELLED;
  }

  /////if (tracer_debug)
  /////  printf("Start point=(%f,%f) Label=%f \n",start_pt->x,start_pt->y,label);

  Tr->horiz[Tr->size]  = start_pt->x;
  Tr->vert[Tr->size] = start_pt->y;
//  Tr->label[Tr->size] = (int)label;

  temp_pos = Tr->size;

  Tr->size++;
  s_dir = 6; dir = s_dir;

  contour_length=1;

  while (1) {
    if (contour_length == 1){
      dir=5;
      dir_sign=1;
    }
    else if (contour_length > 1)
      dir=find_initial_tracer_direction(curr,prev,&dir_sign);

    /////if (tracer_debug)
    /////  printf("before: curr->y = %1.16f and curr->x = %1.16f\n", curr_pt.y, curr_pt.x);
    next_xing_location(&curr_pt,&next_pt,&end_pt,xings->vert,xings->horiz,
           mod(dir,8),label,height,width);


    if (next_pt.x < 0.0) {
      for (i=1;i<=4;i++) {
        next_dir = dir+dir_sign*i;
        /*if (debug_dir_flag){
          printf("Next Dir=%d Next Dir Mod=%d\n", next_dir,mod(next_dir,8));
          }*/
        next_xing_location(&curr_pt,&next_pt,&end_pt,xings->vert,xings->horiz,
               mod(next_dir,8),label,height,width);
        if (next_pt.x >= 0.0)
          break;
        else {
          next_dir = dir-dir_sign*i;
          /*if (debug_dir_flag){
            printf("Next Dir=%d Next Dir Mod=%d\n", next_dir,mod(next_dir,8));
            }*/
          next_xing_location(&curr_pt,&next_pt,&end_pt,xings->vert,
                 xings->horiz,mod(next_dir,8),label,height,width);
          if (next_pt.x >= 0.0) {
            break;
          }
        }
      }/*for (i=1;i<=4;i++)*/
    }
    /* if the next point is another contour then put that point as the
       end point */


    /* look one more pixel away due to problems in detection xings */
    if (next_pt.x < 0.0) {
      /////if (tracer_debug)
      /////  printf("breaking: next pts: (%f, %f)\n", next_pt.y, next_pt.x);
      break;
    }
    prev.x=curr_pt.x;
    prev.y=curr_pt.y;
    curr_pt.x = next_pt.x;
    curr_pt.y = next_pt.y;
    curr.x = next_pt.x;
    curr.y = next_pt.y;

    Tr->horiz[Tr->size] = next_pt.x;
    Tr->vert[Tr->size]  = next_pt.y;
//    Tr->label[Tr->size]  = (int)label;
    Tr->size++;
    contour_length++;
  } /*while(1)*/


  /////if (tracer_debug)
  /////  printf("label =%i and end point: (%f,%f)\n", (int) label, curr_pt.y, curr_pt.x);

#ifdef TEST
  if (contour_length < 2) {
    Tr->size =temp_pos;
    for(ii=0; ii<=contour_length; ii++) {
      Tr->horiz[temp_pos] = next_pt.x;
      Tr->vert[temp_pos]  = next_pt.y;
      temp_pos++;
    }
  }
#endif
  /* printf("Label =%d Contour_length=%d\n",(int)label,contour_length); */
  Tr->horiz[Tr->size] = ENDOFCONTOUR;
  Tr->vert[Tr->size] = ENDOFCONTOUR;
  Tr->size++;

}

#if 0
/* The following is useless bogus code; but might be useful to check it out for
 * historical reasons */
#define MERGED -9.0

void 
merge_same_contours(Tracer *tracer, Tracer *tracer_final) 
{
  int c1, c2, merged;
  int ii,jj,kk,ll;
  double dist;
  double start_x1, start_y1, start_x2, start_y2;
  double end_x1, end_y1;

  

  /*if (debug_flag){
    kk =0;
    while (tracer->vert[kk] != ENDOFLIST) {
      printf("kk= %i Pt(%f,%f) Label=%d\n", kk, tracer->vert[kk], 
       tracer->horiz[kk],tracer->label[kk]);
      kk++;
    }
    printf("End of contours before merge\n");
  }*/

  ii=0; ll=0; c1 = 1; c2 = 1;
  while(tracer->vert[ii] != ENDOFLIST) {
    start_y1 = tracer->vert[ii];
    start_x1 = tracer->horiz[ii];
    merged = 0;
    /* loop over the first contour in order to find the last item of the contour */
    while (tracer->vert[ii] != ENDOFCONTOUR) {
      end_y1 = tracer->vert[ii];
      end_x1 = tracer->horiz[ii];
      ii++;
    }
    dist = sqrt(sqr(start_y1-end_y1)+sqr(start_x1-end_x1));
    jj = ii-1;
    ii++;
    c1++;
    c2 = c1;
    kk = ii;
    while (tracer->vert[kk] != ENDOFLIST && start_y1 != MERGED && dist > 1.5) {
      start_y2 = tracer->vert[kk];
      start_x2 = tracer->horiz[kk];
      if (start_y1 == start_y2 && start_x1 == start_x2) {
        merged  = 1;
        while(jj >=0 && tracer->vert[jj] != ENDOFCONTOUR) {
          tracer_final->vert[ll] = tracer->vert[jj];
          tracer_final->horiz[ll] = tracer->horiz[jj];
//          tracer_final->label[ll] = tracer->label[jj];
          ll++; jj--;
        }
        tracer->vert[kk] = MERGED;
        kk++;
        while(tracer->vert[kk] != ENDOFCONTOUR) {
          tracer_final->vert[ll] = tracer->vert[kk];
          tracer_final->horiz[ll] = tracer->horiz[kk];
//          tracer_final->label[ll] = tracer->label[kk];
          ll++; kk++;
        }
        tracer_final->vert[ll] = ENDOFCONTOUR;
        tracer_final->horiz[ll] = ENDOFCONTOUR;
//        tracer_final->label[ll] = ENDOFCONTOUR;
        ll++; kk++;
        break;
      }
      else {
        while(tracer->vert[kk] != ENDOFCONTOUR) {
          kk++;
        }
        kk++;
      }
    }
    if (merged == 0 && start_y1 != MERGED) {
     while(tracer->vert[jj] != ENDOFCONTOUR && jj >=0 ) {
       tracer_final->vert[ll] = tracer->vert[jj];
       tracer_final->horiz[ll] = tracer->horiz[jj];
//       tracer_final->label[ll] = tracer->label[jj];
       ll++; jj--;
     } 
     tracer_final->vert[ll] = ENDOFCONTOUR;
     tracer_final->horiz[ll] = ENDOFCONTOUR;
//     tracer_final->label[ll] = ENDOFCONTOUR;
     ll++;
    }
    if (c1 > 2000) 
      break;
  }
  
  tracer_final->vert[ll] = ENDOFLIST;
  tracer_final->horiz[ll] = ENDOFLIST;   
//  tracer_final->label[ll] = ENDOFLIST;

  tracer_final->size = ll++;

  /*if (debug_flag){
    kk =0;
    while (tracer_final->vert[kk] != ENDOFLIST) {
      printf("kk= %i Pt(%f,%f) Label=%d\n", kk, tracer_final->vert[kk], 
       tracer_final->horiz[kk],tracer_final->label[kk]);
      kk++;
    }
  }*/
}
#endif

int
mod(int number, int base)
{
  float t1,t2,t3;
  
  if ((number >= 0) && (number < base)) 
   return (number);
  else if (static_cast<int>(vnl_math::abs(number)) == base)
   return(0);
  else if (number > base){
   t1 = (float) number/(float) base;
   t2 = (int) t1;
   t3 = (t1-t2);
   return((int)(base*t3 +0.5));
  } else //if (number < 0) {
   return ((int) (base+number));
}

//: Allocate memory for the tracer.
//  Input : Tr - Tracer structure.
//          size - size of memory to be allocated.
void 
allocate_data_tracer(Tracer *Tr, int size)
{
  Tr->vert=(double *)calloc(size,sizeof(double));
  Tr->horiz=(double *)calloc(size,sizeof(double));
//  Tr->label=(int *)calloc(size,sizeof(int));
//  Tr->id=(int *)calloc(size,sizeof(int));
  Tr->length = (int *) calloc(size, sizeof(int));
}
