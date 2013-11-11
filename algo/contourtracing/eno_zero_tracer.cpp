#include <vcl_cmath.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "1d_eno_types.h"
#include "utils.h"
#include "image_processing.h"
#include "redt.h"


#include "graphics.h"

/***************************************************************************
  Description: Trace the ENO zero crossings of a surface. Does ENO on the
               input surface and then passes the ENO zero crossings to the
         tracer.
  Inputs :  surface - The surface whose zerox are to be traced.
            label - array of labels. Different labels represent different
      objects.
      height,width - image dimensions
  Output: Tr - The traced ENO Zerox.
***************************************************************************/


void  trace_eno_zero_xings_labelled(double *surface, Tracer *Tr, int *label,
            int height, int width) 

{
  int y,x,pos,ii,jj,id;
  int contours_ptr=0;
  int no_of_contours,pts_ina_contour;
  double yd,xd;
  ENO_Interval_Image_t *enoimage;
  Xings xings;
  double *zerox,count;
  FILE *fopen(), *f1;

  // enoimage = enoimage_from_data_tbs(surface,height,width);
  enoimage = enoimage_from_data(surface,height,width);
  eno_label_zerox(enoimage,surface,label);
  place_zerox_by_label(enoimage,surface,label, height, width); 

  xings.vert = (XingsLoc *) calloc(height*width,sizeof(XingsLoc));
  xings.horiz = (XingsLoc *) calloc(height*width,sizeof(XingsLoc));
  zerox=(double*)calloc(height*width,sizeof(double));

  ii=0; 
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      xings.horiz[pos].loc[1] = NOXING;
      xings.vert[pos].loc[1] = NOXING;
      xings.horiz[pos].loc[0] = NOXING;
      xings.vert[pos].loc[0] = NOXING;
      xings.horiz[pos].cnt = 0;
      if (enoimage->horiz[pos].zerox){
        xings.horiz[pos].loc[0] =  enoimage->horiz[pos].zerox->loc[0];
  xings.horiz[pos].label[0] =  enoimage->horiz[pos].zerox->label[0];
        xings.horiz[pos].cnt = 1;
        contours_ptr++;
        if (enoimage->horiz[pos].zerox->cnt == 2) {
          xings.horiz[pos].loc[1] =  enoimage->horiz[pos].zerox->loc[1];
          xings.horiz[pos].label[1] =  enoimage->horiz[pos].zerox->label[1];
          xings.horiz[pos].cnt = 2;
          contours_ptr++;
        }
      }
      if (enoimage->vert[pos].zerox) {
        xings.vert[pos].loc[0] =  enoimage->vert[pos].zerox->loc[0];
        xings.vert[pos].label[0] =  enoimage->vert[pos].zerox->label[0];
        xings.vert[pos].cnt = 1;
        contours_ptr++;
        if (enoimage->vert[pos].zerox->cnt == 2) {
          xings.vert[pos].loc[1] =  enoimage->vert[pos].zerox->loc[1];
          xings.vert[pos].label[1] =  enoimage->vert[pos].zerox->label[1];
          xings.vert[pos].cnt = 2;
          contours_ptr++;
        }
      }
    }
  }

  /*pos=80*width+28;
    printf("Vert: %f %d %d \n",xings.vert[pos].loc[0],xings.vert[pos].label[0],
    xings.vert[pos].cnt);
    printf("Horiz: %f %d %d \n",xings.horiz[pos].loc[0],
    xings.horiz[pos].label[0], xings.horiz[pos].cnt);
    printf("Horiz: %f %d %d \n",xings.horiz[pos].loc[1],
    xings.horiz[pos].label[1], xings.horiz[pos].cnt);
    printf("EnoHoriz: %f %d %d \n",enoimage->horiz[pos].zerox->loc[0],
    enoimage->horiz[pos].zerox->label[0], enoimage->horiz[pos].zerox->cnt);
    printf("EnoHoriz: %f %d %d \n",enoimage->horiz[pos].zerox->loc[1],
    enoimage->horiz[pos].zerox->label[1], enoimage->horiz[pos].zerox->cnt);*/

  ii=0;
  /*f1=fopen("/home/tbs/eno-zerox.dat","w");
    for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      if (enoimage->horiz[pos].zerox){
  zerox[ii]=enoimage->horiz[pos].zerox->loc[0];
  zerox[ii+1]=(double)y;
  ii++;ii++;
      
  if (enoimage->horiz[pos].zerox->cnt == 2) {
    zerox[ii]=enoimage->horiz[pos].zerox->loc[1];
    zerox[ii+1]=(double)y;
    ii++;ii++;
  }
      }
      if (enoimage->vert[pos].zerox) {
  zerox[ii]=(double)x;
  zerox[ii+1]=enoimage->vert[pos].zerox->loc[0];
  ii++;ii++;
  
  if (enoimage->vert[pos].zerox->cnt == 2) {     
    zerox[ii]=(double)x;
    zerox[ii+1]=enoimage->vert[pos].zerox->loc[1];
    ii++;ii++;
  }
      }
    }
  }

  count=(double)contours_ptr;
  fwrite(&count,sizeof(double),1,f1);
  fwrite(zerox,sizeof(double),2*contours_ptr,f1);
  fclose(f1);*/

  /* allocate memory for tracer */ 
  contours_ptr = height*width;
  contours_ptr = height*width;

  /*Tr->vert = (double *) calloc(contours_ptr,sizeof(double));
    Tr->horiz = (double *) calloc(contours_ptr,sizeof(double));
    Tr->label = (int *) calloc(contours_ptr,sizeof(int));*/
  allocate_data_tracer(Tr,contours_ptr);
  
  subpixel_contour_tracer_all(Tr, &xings, height, width); 
  /* label_traced_contours(Tr,enoimage); */
  fix_tracer_labels(Tr);

  ii=0;
  no_of_contours=0; pts_ina_contour=0;
  while(Tr->vert[ii] != ENDOFLIST && Tr->horiz[ii] != ENDOFLIST){
    pts_ina_contour=0;
    no_of_contours++;
    while(Tr->vert[ii] != ENDOFCONTOUR && Tr->horiz[ii] != ENDOFCONTOUR){
      pts_ina_contour++;
      ii++;
    }
    /* printf("Found a contour of length %d\n",pts_ina_contour); */
    ii++;
  }
  printf("Found %d contour(s)\n",no_of_contours);

  enoimage_dealloc_data(enoimage);
  free(enoimage);
  free(xings.vert);
  free(xings.horiz);
  free(zerox);
  
}

/***************************************************************************
//:  Description: Trace the ENO zero crossings of a surface. Does ENO on the
input surface and then passes the ENO zero crossings to the tracer. Same as
above, except there is  no label array.

Inputs :  
  surface - The surface whose zerox are to be traced.

Output: 
  Tr - The traced ENO Zerox.
***************************************************************************/
int  
trace_eno_zero_xings(double *surface, Tracer *Tr,int height, int width) 
{
  int y,x,pos,ii,jj,id;
  int contours_ptr=0;
  int *label;
  int debug_flag=0;
  int no_of_contours,pts_ina_contour;
  double yd,xd;
  ENO_Interval_Image_t *enoimage;
  Xings xings;
  double *zerox,count;

  label=(int*)calloc(height*width,sizeof(int));
  for (ii=0;ii<height*width;ii++)
    label[ii]=-15;

  enoimage = enoimage_from_data(surface,height,width);
  eno_label_zerox(enoimage,surface,label);
  free(label);

  xings.vert = (XingsLoc *) calloc(height*width,sizeof(XingsLoc));
  xings.horiz = (XingsLoc *) calloc(height*width,sizeof(XingsLoc));
  zerox=(double*)calloc(height*width,sizeof(double));
  

  // Copy zero crossing info from enoimage to Xings structure
  ii=0; 
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      xings.horiz[pos].loc[1] = NOXING;
      xings.vert[pos].loc[1] = NOXING;
      xings.horiz[pos].loc[0] = NOXING;
      xings.vert[pos].loc[0] = NOXING;
      if (enoimage->horiz[pos].zerox){
        xings.horiz[pos].loc[0] =  enoimage->horiz[pos].zerox->loc[0];
        xings.horiz[pos].label[0] =  enoimage->horiz[pos].zerox->label[0];
        xings.horiz[pos].cnt = 1;
        contours_ptr++;
        if (enoimage->horiz[pos].zerox->cnt == 2) {
          xings.horiz[pos].loc[1] =  enoimage->horiz[pos].zerox->loc[1];
          xings.horiz[pos].label[1] =  enoimage->horiz[pos].zerox->label[1];
          xings.horiz[pos].cnt = 2;
          contours_ptr++;
        }
      }
      if (enoimage->vert[pos].zerox) {
        xings.vert[pos].loc[0] =  enoimage->vert[pos].zerox->loc[0];
        xings.vert[pos].label[0] =  enoimage->vert[pos].zerox->label[0];
        xings.vert[pos].cnt = 1;
        contours_ptr++;
        if (enoimage->vert[pos].zerox->cnt == 2) {
          xings.vert[pos].loc[1] =  enoimage->vert[pos].zerox->loc[1];
          xings.vert[pos].label[1] =  enoimage->vert[pos].zerox->label[1];
          xings.vert[pos].cnt = 2;
          contours_ptr++;
        }
      }
    }
  }

  /*printf("Done Copying to Tek's Structure\n");
    fflush(stdout);*/


  enoimage_dealloc_data(enoimage);
  free(enoimage);

  /* allocate memory for tracer */ 
  contours_ptr = height*width;

  allocate_data_tracer(Tr,contours_ptr);

  printf("Going to Trace Points\n");
  subpixel_contour_tracer_all(Tr, &xings, height, width); 
  printf("Done Trace Points\n");

  ii=0;
  no_of_contours=0; pts_ina_contour=0;
  while(Tr->vert[ii] != ENDOFLIST && Tr->horiz[ii] != ENDOFLIST) {
    pts_ina_contour=0;
    while(Tr->vert[ii] != ENDOFCONTOUR && Tr->horiz[ii] != ENDOFCONTOUR) {
      pts_ina_contour++; ii++;
    }
    Tr->length[no_of_contours] = pts_ina_contour;
    /* printf("Found a contour of length %d\n",pts_ina_contour); */
    ii++; no_of_contours++;
  }
  printf("Found %d contour(s)\n",no_of_contours);
 
  free(zerox); free(xings.vert); free(xings.horiz); 

  return no_of_contours;
}

/**********************************************************************************
       Description :Find the traced outline of a binary image, using the distance 
       transform and then using the subpixel contour tracer.
       Inputs : image_array  -> image
                height,width -> image dimensions
    label -> label that is to be traced.
       Output : contour_list -> list of traced contour points
 
 **********************************************************************************/


void contour_tracer_using_signed_dt(int label, int *image_array, 
            double *contour_list, double smoothing,
            int height, int width)
{
  float *float_surface_array;
  double *surface_array;
  double prevx,prevy;
  unsigned char *uchar_image_array;
  int *label_array;
  int i,j,size;
  int pos,pos2,count;
  Tracer Tr;
  FILE *fp;

  size=height*width;
  label_array=(int *)calloc(size,sizeof(int)); 
  float_surface_array=(float *)calloc(size,sizeof(float)); 
  surface_array=(double *)calloc(size,sizeof(double)); 
  uchar_image_array = (unsigned char *)calloc(size,sizeof(unsigned char));

  for (i=0;i<height;i++){
    for (j=0;j<width;j++){
      if (image_array[i*width+j] == label)
  uchar_image_array[i*width+j] = 1;
      else
  uchar_image_array[i*width+j] = 0;
    }
  }

  /*fp=fopen("/home/tbs/image.dat","w");
    fwrite(uchar_image_array,sizeof(unsigned char),size,fp);
    fclose(fp);*/

  signed_distance_transform(uchar_image_array, float_surface_array,height, width);
  /*fp=fopen("/home/tbs/surf.dat","w");
    fwrite(float_surface_array,sizeof(float),size,fp);
    fclose(fp);*/
  if (smoothing > 0.0){
    /* printf("Doing curvature smoothing %f\n",smoothing); */
    CopyIm_FloatToDouble(float_surface_array,surface_array,height,width);
    curvature_smoothing(surface_array, 0.3,(int)smoothing,0, height,width);
  }
  else{
    CopyIm_FloatToDouble(float_surface_array,surface_array,height,width);
  }

  trace_eno_zero_xings(surface_array,&Tr,height,  width);  
  
  pos=0;pos2=0;
  while(Tr.vert[pos] != ENDOFLIST  && Tr.horiz[pos] != ENDOFLIST ){
    count=0;
    while(Tr.vert[pos] != ENDOFCONTOUR  && Tr.horiz[pos] != ENDOFCONTOUR ){
      if (count == 0){
  count++;
  contour_list[pos2]=Tr.horiz[pos];
  contour_list[pos2+1]=Tr.vert[pos];
  prevx = Tr.horiz[pos];
  prevy = Tr.vert[pos];
  /*printf("(%5.2f %5.2f) (%5.2f %5.2f)\n",Tr.horiz[pos],Tr.vert[pos],
    contour_list[pos2],contour_list[pos2+1]);*/
  pos2++;pos2++;
  pos++;
      }
      else{
  if (fabs(prevx-Tr.horiz[pos]) < 0.2 && fabs(prevy-Tr.vert[pos]) < 0.5){
    prevx = Tr.horiz[pos];
    prevy = Tr.vert[pos];
/*     printf("(%5.2f %5.2f)\n",Tr.horiz[pos],Tr.vert[pos]); */
    pos++;
  }
  else{
    contour_list[pos2]=Tr.horiz[pos];
    contour_list[pos2+1]=Tr.vert[pos];
    prevx = Tr.horiz[pos];
    prevy = Tr.vert[pos];
    /*printf("(%5.2f %5.2f) (%5.2f %5.2f)\n",Tr.horiz[pos],Tr.vert[pos],
      contour_list[pos2],contour_list[pos2+1]);*/
    pos2++;pos2++;
    pos++;
  }
      }
    }
    contour_list[pos2]=ENDOFCONTOUR;
    contour_list[pos2+1]=ENDOFCONTOUR;
    pos2++;pos2++;
    pos++;
  }
  contour_list[pos2]=ENDOFLIST;
  contour_list[pos2+1]=ENDOFLIST;
  
  free(label_array);
  free(uchar_image_array);
  free(float_surface_array);
  free(surface_array);
  free_data_tracer(&Tr);

}

/***************************************************************************
  Description: Trace the ENO zero crossings of a surface. Does ENO on the
               input surface and then passes the ENO zero crossings to the
         tracer. Same as above programs, except that the  label array
         is not an input, but is computed 
  Inputs :  surface - The surface whose zerox are to be traced.
      height,width - image dimensions
  Output: Tr - The traced ENO Zerox.
***************************************************************************/
void  trace_eno_zero_xings_create_labels(double *surface, Tracer *Tr,int height, int width) 

{
  int y,x,pos,ii,jj,id;
  int contours_ptr=0;
  int *label;
  int debug_flag=0;
  int no_of_contours,pts_ina_contour;
  double yd,xd;
  ENO_Interval_Image_t *enoimage;
  Xings xings;
  double *zerox,count;
  FILE  *f1; //*fopen(),

  label=(int*)calloc(height*width,sizeof(int));

  
  enoimage = enoimage_from_data(surface,height,width);
  fill_regions(enoimage, label,surface);
  eno_label_zerox(enoimage,surface,label);



  xings.vert = (XingsLoc *) calloc(height*width,sizeof(XingsLoc));
  xings.horiz = (XingsLoc *) calloc(height*width,sizeof(XingsLoc));
  zerox=(double*)calloc(height*width,sizeof(double));
  

  ii=0; 
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      xings.horiz[pos].loc[1] = NOXING;
      xings.vert[pos].loc[1] = NOXING;
      xings.horiz[pos].loc[0] = NOXING;
      xings.vert[pos].loc[0] = NOXING;
      if (enoimage->horiz[pos].zerox){
        xings.horiz[pos].loc[0] =  enoimage->horiz[pos].zerox->loc[0];
        xings.horiz[pos].label[0] =  enoimage->horiz[pos].zerox->label[0];
        xings.horiz[pos].cnt = 1;
        contours_ptr++;
        if (enoimage->horiz[pos].zerox->cnt == 2) {
          xings.horiz[pos].loc[1] =  enoimage->horiz[pos].zerox->loc[1];
          xings.horiz[pos].label[1] =  enoimage->horiz[pos].zerox->label[1];
          xings.horiz[pos].cnt = 2;
          contours_ptr++;
        }
      }
      if (enoimage->vert[pos].zerox) {
        xings.vert[pos].loc[0] =  enoimage->vert[pos].zerox->loc[0];
        xings.vert[pos].label[0] =  enoimage->vert[pos].zerox->label[0];
        xings.vert[pos].cnt = 1;
        contours_ptr++;
        if (enoimage->vert[pos].zerox->cnt == 2) {
          xings.vert[pos].loc[1] =  enoimage->vert[pos].zerox->loc[1];
          xings.vert[pos].label[1] =  enoimage->vert[pos].zerox->label[1];
          xings.vert[pos].cnt = 2;
          contours_ptr++;
        }
      }
    }
  }

  
  if (debug_flag){
    ii=0;
    f1=fopen("/home/tbs/eno-zerox.dat","w");
    for(y=0; y<height; y++) {
      for(x=0; x<width; x++) {
  pos = y*width+x;
  if (enoimage->horiz[pos].zerox){
    zerox[ii]=enoimage->horiz[pos].zerox->loc[0];
    zerox[ii+1]=(double)y;
    ii++;ii++;
    
    if (enoimage->horiz[pos].zerox->cnt == 2) {
      zerox[ii]=enoimage->horiz[pos].zerox->loc[1];
      zerox[ii+1]=(double)y;
      ii++;ii++;
    }
  }
  if (enoimage->vert[pos].zerox) {
    zerox[ii]=(double)x;
    zerox[ii+1]=enoimage->vert[pos].zerox->loc[0];
    ii++;ii++;
    
    if (enoimage->vert[pos].zerox->cnt == 2) {     
      zerox[ii]=(double)x;
      zerox[ii+1]=enoimage->vert[pos].zerox->loc[1];
      ii++;ii++;
    }
  }
      }
    }
    

    count=(double)contours_ptr;
    fwrite(&count,sizeof(double),1,f1);
    fwrite(zerox,sizeof(double),2*contours_ptr,f1);
    fclose(f1);
  }
  /* allocate memory for tracer */ 
  contours_ptr = height*width;
  contours_ptr = height*width;

  /*Tr->vert = (double *) calloc(contours_ptr,sizeof(double));
    Tr->horiz = (double *) calloc(contours_ptr,sizeof(double));
    Tr->label = (int *) calloc(contours_ptr,sizeof(int));*/
  allocate_data_tracer(Tr,contours_ptr);

  if(debug_flag)
    printf("Tr->label%x\n",Tr->label);
  
  subpixel_contour_tracer_all(Tr, &xings, height, width); 
  
  ii=0;
  no_of_contours=0; pts_ina_contour=0;
  while(Tr->vert[ii] != ENDOFLIST && Tr->horiz[ii] != ENDOFLIST){
    pts_ina_contour=0;
    no_of_contours++;
    while(Tr->vert[ii] != ENDOFCONTOUR && Tr->horiz[ii] != ENDOFCONTOUR){
      pts_ina_contour++;
      ii++;
    }
    printf("Found a contour of length %d\n",pts_ina_contour);
    ii++;
  }
  printf("Found %d contour(s)\n",no_of_contours);

  enoimage_dealloc_data(enoimage);
  free(enoimage);
  free(zerox);
  free(label);
  free(xings.vert);
  free(xings.horiz);
}

/**********************************************************************************
       Description :Find the ENO zero_xings of a surface. Wrapper around
       the C version of "trace_eno_zero_xings" to avoid having to swig
       the "Tracer" structure.
       Inputs : image_array  -> image
                height,width -> image dimensions
    label -> label that is to be traced.
       Output : contour_list -> list of traced contour points
**********************************************************************************/
void trace_eno_zero_xings_raph(double *surface, double *contour_list,
             int height, int width)
{
  double prevx,prevy;
  int i,j,size;
  int pos,pos2,count;
  Tracer Tr;
  FILE *fp;

  size=height*width;

  trace_eno_zero_xings(surface,&Tr,height,width);  
  
  pos=0;pos2=0;
  while(Tr.vert[pos] != ENDOFLIST  && Tr.horiz[pos] != ENDOFLIST ){
    count=0;
    while(Tr.vert[pos] != ENDOFCONTOUR  && Tr.horiz[pos] != ENDOFCONTOUR ){
      if (count == 0){
  count++;
  contour_list[pos2]=Tr.horiz[pos];
  contour_list[pos2+1]=Tr.vert[pos];
  prevx = Tr.horiz[pos];
  prevy = Tr.vert[pos];
  /*printf("(%5.2f %5.2f) (%5.2f %5.2f)\n",Tr.horiz[pos],Tr.vert[pos],
    contour_list[pos2],contour_list[pos2+1]);*/
  pos2++;pos2++;
  pos++;
      }
      else{
  if (fabs(prevx-Tr.horiz[pos]) < 0.2 && fabs(prevy-Tr.vert[pos]) < 0.5){
    prevx = Tr.horiz[pos];
    prevy = Tr.vert[pos];
/*     printf("(%5.2f %5.2f)\n",Tr.horiz[pos],Tr.vert[pos]); */
    pos++;
  }
  else{
    contour_list[pos2]=Tr.horiz[pos];
    contour_list[pos2+1]=Tr.vert[pos];
    prevx = Tr.horiz[pos];
    prevy = Tr.vert[pos];
    /*printf("(%5.2f %5.2f) (%5.2f %5.2f)\n",Tr.horiz[pos],Tr.vert[pos],
      contour_list[pos2],contour_list[pos2+1]);*/
    pos2++;pos2++;
    pos++;
  }
      }
    }
    contour_list[pos2]=ENDOFCONTOUR;
    contour_list[pos2+1]=ENDOFCONTOUR;
    pos2++;pos2++;
    pos++;
  }
  contour_list[pos2]=ENDOFLIST;
  contour_list[pos2+1]=ENDOFLIST;
  
  free_data_tracer(&Tr);

}

/**********************************************************************************
       Description :Find the ENO zero_xings of a surface. Wrapper around
       the C version of "trace_eno_zero_xings" to avoid having to swig
       the "Tracer" structure.
       Inputs : image_array  -> image
                height,width -> image dimensions
    label -> label that is to be traced.
       Output : contour_list -> list of traced contour points
**********************************************************************************/
void trace_eno_zero_xings_labelled_raph(double *surface, double *contour_list,
          int *label_array, int height, int width)
{
  double prevx,prevy;
  int i,j,size;
  int pos,pos2,count;
  Tracer Tr;
  FILE *fp;

  size=height*width;

  trace_eno_zero_xings_labelled(surface,&Tr,label_array,height,width);  
  
  pos=0;pos2=0;
  while(Tr.vert[pos] != ENDOFLIST  && Tr.horiz[pos] != ENDOFLIST ){
    count=0;
    while(Tr.vert[pos] != ENDOFCONTOUR  && Tr.horiz[pos] != ENDOFCONTOUR ){
      if (count == 0){
  count++;
  contour_list[pos2]=Tr.horiz[pos];
  contour_list[pos2+1]=Tr.vert[pos];
  prevx = Tr.horiz[pos];
  prevy = Tr.vert[pos];
  /*printf("(%5.2f %5.2f) (%5.2f %5.2f)\n",Tr.horiz[pos],Tr.vert[pos],
    contour_list[pos2],contour_list[pos2+1]);*/
  pos2++;pos2++;
  pos++;
      }
      else{
  if (fabs(prevx-Tr.horiz[pos]) < 0.2 && fabs(prevy-Tr.vert[pos]) < 0.5){
    prevx = Tr.horiz[pos];
    prevy = Tr.vert[pos];
/*     printf("(%5.2f %5.2f)\n",Tr.horiz[pos],Tr.vert[pos]); */
    pos++;
  }
  else{
    contour_list[pos2]=Tr.horiz[pos];
    contour_list[pos2+1]=Tr.vert[pos];
    prevx = Tr.horiz[pos];
    prevy = Tr.vert[pos];
    /*printf("(%5.2f %5.2f) (%5.2f %5.2f)\n",Tr.horiz[pos],Tr.vert[pos],
      contour_list[pos2],contour_list[pos2+1]);*/
    pos2++;pos2++;
    pos++;
  }
      }
    }
    contour_list[pos2]=ENDOFCONTOUR;
    contour_list[pos2+1]=ENDOFCONTOUR;
    pos2++;pos2++;
    pos++;
  }
  contour_list[pos2]=ENDOFLIST;
  contour_list[pos2+1]=ENDOFLIST;
  
  free_data_tracer(&Tr);

}
