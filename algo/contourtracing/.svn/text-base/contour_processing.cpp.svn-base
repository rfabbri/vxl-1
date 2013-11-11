#include <stdio.h>
#include <vcl_cmath.h>

#include "graphics.h"

#define MERGED -9.0

void merge_same_contours(Tracer *tracer, Tracer *tracer_final) 

{
  int y,x,pos;
  int c1, c2, merged;
  int ii,jj,kk,ll;
  int debug_flag=0;
  double yf,xf,dist;
  double start_x1, start_y1, start_x2, start_y2;
  double end_x1, end_y1, end_x2, end_y2;

  

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
          tracer_final->label[ll] = tracer->label[jj];
          ll++; jj--;
        }
        tracer->vert[kk] = MERGED;
        kk++;
        while(tracer->vert[kk] != ENDOFCONTOUR) {
          tracer_final->vert[ll] = tracer->vert[kk];
          tracer_final->horiz[ll] = tracer->horiz[kk];
          tracer_final->label[ll] = tracer->label[kk];
          ll++; kk++;
        }
        tracer_final->vert[ll] = ENDOFCONTOUR;
        tracer_final->horiz[ll] = ENDOFCONTOUR;
  tracer_final->label[ll] = ENDOFCONTOUR;
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
       tracer_final->label[ll] = tracer->label[jj];
       ll++; jj--;
     } 
     tracer_final->vert[ll] = ENDOFCONTOUR;
     tracer_final->horiz[ll] = ENDOFCONTOUR;
     tracer_final->label[ll] = ENDOFCONTOUR;
     ll++;
    }
    if (c1 > 2000) 
      break;
  }
  
  tracer_final->vert[ll] = ENDOFLIST;
  tracer_final->horiz[ll] = ENDOFLIST;   
  tracer_final->label[ll] = ENDOFLIST;

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


void label_traced_contours(Tracer *Tr, ENO_Interval_Image_t *enoimage)
{
  int ii,kk;
  int label,pt_type;
  int x,y;
  int height,width;
  double fx,fy;
  Tr_Point contour_pt; 
  height=enoimage->height;
  width=enoimage->width;

  ii=0;kk=0;
  while(Tr->vert[ii] != ENDOFLIST) {
    while (1){
      if (Tr->vert[ii] == ENDOFCONTOUR){
  Tr->label[kk]=UNLABELLED;
  kk++;
  break;
      }
      contour_pt.x=Tr->horiz[ii];
      contour_pt.y=Tr->vert[ii];
      pt_type=tr_xing_type(&contour_pt);
      x=(int)floor(contour_pt.x);
      y=(int)floor(contour_pt.y);
      /////printf("(%f,%f)\n",contour_pt.x,contour_pt.y);
      if (pt_type == TR_VERT){
  if (enoimage->vert[y*width+x].zerox){
    if(enoimage->vert[y*width+x].zerox->cnt == 1 &&
       enoimage->vert[y*width+x].zerox->label[0] != UNLABELLED){
      Tr->label[kk]=enoimage->vert[y*width+x].zerox->label[0];
      kk++;
      break;
    }
  }
      }
      else if (pt_type == TR_HORIZ){
  if (enoimage->horiz[y*width+x].zerox){
    if (enoimage->horiz[y*width+x].zerox->cnt == 1 &&
        enoimage->horiz[y*width+x].zerox->label[0] != UNLABELLED){
      Tr->label[kk]=enoimage->horiz[y*width+x].zerox->label[0];
      kk++;
      break;
    }
  }
      }
      ii++;
    }
    while(Tr->vert[ii] != ENDOFCONTOUR){
      ii++;
    }
    ii++;
  }
  
}

void remove_xings_due_to_padding(Tracer *Tr, int height, int width, int pad)
{
  Tracer tmp;
  int ii,jj;

  allocate_data_tracer(&tmp,height*width);
  
  ii=0;
  while(Tr->vert[ii] != ENDOFLIST){
    tmp.vert[ii]=Tr->vert[ii];
    tmp.horiz[ii]=Tr->horiz[ii];
    tmp.label[ii]=Tr->label[ii];
    ii++;
  }
  tmp.vert[ii]=ENDOFLIST;
  tmp.horiz[ii]=ENDOFLIST;
  tmp.label[ii]=ENDOFLIST;
  
  ii=0;jj=0;
  while(tmp.vert[ii] != ENDOFLIST){
    while(tmp.vert[ii] != ENDOFCONTOUR){
      if (tmp.vert[ii] >= pad && tmp.vert[ii] <= height-pad-1 &&
    tmp.horiz[ii]  >= pad && tmp.horiz[ii] <= width-pad-1 ){
  Tr->vert[jj]=tmp.vert[ii];
  Tr->horiz[jj]=tmp.horiz[ii];
  Tr->label[jj]=tmp.label[ii];
  jj++;
      }
      ii++;
    }
    Tr->vert[jj]= ENDOFCONTOUR;
    Tr->horiz[jj]= ENDOFCONTOUR;
    Tr->label[jj]= ENDOFCONTOUR;
    ii++;jj++;
  }
  Tr->vert[jj]=ENDOFLIST;
  Tr->horiz[jj]=ENDOFLIST;
  Tr->label[jj]=ENDOFLIST;
  free_data_tracer(&tmp);
}

void fix_tracer_labels(Tracer *Tr)
{
  int ii,jj;
  int contour_label;

  ii=0;jj=0;
  while(Tr->vert[ii] != ENDOFLIST){
    contour_label=-10;
    jj=ii;
    while(Tr->vert[ii] != ENDOFCONTOUR){  
      if (contour_label < 0 && Tr->label[ii] > 0){
  contour_label=Tr->label[ii];
      }
      ii++;
    }
    if (contour_label > 0){
      while(Tr->vert[jj] != ENDOFCONTOUR){  
  if (Tr->label[jj] <= 0){
    Tr->label[jj]=contour_label;
  }
  jj++;
      }
    }
    ii++;
  }
}

void place_zerox_by_label(ENO_Interval_Image_t *enoimage, double *surface,
        int *region_label_array, int height, int width)
{

  int x,y;
  int label, reg_label, label_xp, label_yp;
  int verbose=0;

  for (y=1;y<height-1;y++){
    for (x=1;x<width-1;x++){
      label=region_label_array[y*width+x];
      label_yp=region_label_array[(y+1)*width+x];
      label_xp=region_label_array[y*width+x+1];
      /*if (x==129 && y==72){
  verbose=1;
  }
  else{
  verbose=0;
  }
  if (verbose){
  printf("Loc=(%d,%d) Lb=%d Lbx=%d Lby=%d\n",x,y,label,label_xp,
  label_yp);
  }*/
      /* Check for zero along hor. grid line */ 
      if (label != label_xp){
  /* One of the labels is the background -> place one zerox */
  if (label == 0 || label_xp == 0){
    if (enoimage->horiz[y*width+x].zerox ){
      /* do nothing */
    }
    else if (!enoimage->horiz[y*width+x].zerox){
      reg_label = (label>0) ? label:label_xp;
      enoimage->horiz[y*width+x].zerox = enoNEW(ENO_ZeroCrossing_t,1);
      enoimage->horiz[y*width+x].zerox->cnt = 1;
      enoimage->horiz[y*width+x].zerox->loc[0]
        = enoimage->horiz[y*width+x].interp.start + 0.5;
      enoimage->horiz[y*width+x].zerox->label[0] = reg_label;
    }
  }
  else if (label > 0 && label_xp > 0){
    if (enoimage->horiz[y*width+x].zerox &&  
        enoimage->horiz[y*width+x].zerox->cnt == 2 ){
      /* do nothing */
    }
    else if(enoimage->horiz[y*width+x].zerox &&  
      enoimage->horiz[y*width+x].zerox->cnt == 1){
      /* Place one zerox */
      enoimage->horiz[y*width+x].zerox->cnt = 2;
      enoimage->horiz[y*width+x].zerox->loc[0]
        = enoimage->horiz[y*width+x].interp.start + 0.2;
      enoimage->horiz[y*width+x].zerox->label[0] = label;
      enoimage->horiz[y*width+x].zerox->loc[1]
        = enoimage->horiz[y*width+x].interp.start + 0.8;
      enoimage->horiz[y*width+x].zerox->label[1] = label_xp;
    }
    else if(!enoimage->horiz[y*width+x].zerox){
      /* Place two zerox */
      enoimage->horiz[y*width+x].zerox = enoNEW(ENO_ZeroCrossing_t,1);
      enoimage->horiz[y*width+x].zerox->cnt = 2;
      enoimage->horiz[y*width+x].zerox->loc[0]
        = enoimage->horiz[y*width+x].interp.start + 0.2;
      enoimage->horiz[y*width+x].zerox->label[0] = label;
      enoimage->horiz[y*width+x].zerox->loc[1]
        = enoimage->horiz[y*width+x].interp.start + 0.8;
      enoimage->horiz[y*width+x].zerox->label[1] = label_xp;
    }
  }/* if (label > 0 && label_xp > 0)*/
      }/*if (label != label_xp)*/

      /* Check for zero along vertical grid line */ 
      if (label != label_yp){
  /* One of the labels is the background -> place one zerox */
  if (label == 0 || label_yp == 0){
    if (enoimage->vert[y*width+x].zerox ){
      /* do nothing */
    }
    else if (!enoimage->vert[y*width+x].zerox){
      reg_label = (label>0) ? label:label_yp;
      enoimage->vert[y*width+x].zerox = enoNEW(ENO_ZeroCrossing_t,1);
      enoimage->vert[y*width+x].zerox->cnt = 1;
      enoimage->vert[y*width+x].zerox->loc[0]
        = enoimage->vert[y*width+x].interp.start + 0.5;
      enoimage->vert[y*width+x].zerox->label[0] = reg_label;
    }
  }
  else if (label > 0 && label_yp > 0){
    if (enoimage->vert[y*width+x].zerox &&  
        enoimage->vert[y*width+x].zerox->cnt == 2 ){
      /* do nothing */
    }
    else if(enoimage->vert[y*width+x].zerox &&  
      enoimage->vert[y*width+x].zerox->cnt == 1){
      /* Place one zerox */
      enoimage->vert[y*width+x].zerox->cnt = 2;
      enoimage->vert[y*width+x].zerox->loc[0]
        = enoimage->vert[y*width+x].interp.start + 0.2;
      enoimage->vert[y*width+x].zerox->label[0] = label;
      enoimage->vert[y*width+x].zerox->loc[1]
        = enoimage->vert[y*width+x].interp.start + 0.8;
      enoimage->vert[y*width+x].zerox->label[1] = label_yp;
    }
    else if(!enoimage->vert[y*width+x].zerox){
      /* Place two zerox */
      enoimage->vert[y*width+x].zerox = enoNEW(ENO_ZeroCrossing_t,1);
      enoimage->vert[y*width+x].zerox->cnt = 2;
      enoimage->vert[y*width+x].zerox->loc[0]
        = enoimage->vert[y*width+x].interp.start + 0.2;
      enoimage->vert[y*width+x].zerox->label[0] = label;
      enoimage->vert[y*width+x].zerox->loc[1]
        = enoimage->vert[y*width+x].interp.start + 0.8;
      enoimage->vert[y*width+x].zerox->label[1] = label_yp;
    }
  }/* if (label > 0 && label_yp > 0)*/
      }/*if (label != label_yp)*/
    }
  }
}
