#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include <iiointerface.h> 
#include <utils.h>

#include <bubbles.h>
#include "read_image.h"



void read_images(Pars *pars, Bubbles *bubbles)
     
{
  int ii,y,x,pos,err,height,width;
  unsigned char *uchar_image;
  unsigned short *ushort_image;
  char *seedtype;
  extern PICINFO thePInfo; 
  SimpleContour_t* contour;

  FILE *fopen(), *f1;
  char out[128] = "";

  strcpy(out,pars->outputimage);
    
  height = pars->height;
  width  = pars->width;

  if (pars->type == 11) {
     err = ImageIORead(pars->inputimage,&thePInfo);
    if (err == readFailure){
      fprintf(stderr,"could not read file %s. Try another one..\n",pars->inputimage);
      exit(1);
    }
    PadIm_UCharToFloat(thePInfo.pic,bubbles->image_array,(height+2),(width+2));
  }

  if (pars->type == 1) {
    uchar_image  = (unsigned char *) malloc(height*width*sizeof(unsigned char));
    f1 = fopen(pars->inputimage,"r");
    if (f1 == 0) { printf("no such image"); exit(5); }
    fread(uchar_image,sizeof(unsigned char), (height*width),f1);
    fclose(f1);
    PadIm_UCharToFloat(uchar_image,bubbles->image_array,(height+2),(width+2));
    free(uchar_image);
  }
  else if (pars->type == 2) {
    ushort_image = (unsigned short *) malloc(height*width*sizeof(unsigned short));  
    f1 = fopen(pars->inputimage,"r");
    if (f1 == 0) { printf("no such image"); exit(5); }
    fread(ushort_image,sizeof(unsigned short), (height*width),f1);
    fclose(f1);
    PadIm_UShortToFloat(ushort_image,bubbles->image_array,(height+2),(width+2));
    free(ushort_image);
  }  
  
  if (pars->start_from != 0) {
    sprintf(pars->outputimage,"%s-%1.2f-%1.2f-%04d.raw",out,pars->alpha,pars->beta,
            pars->start_from);
    f1 = fopen(pars->outputimage,"r");
    fread(bubbles->surface_array,sizeof(float),((height+2)*(width+2)),f1);
    fclose(f1);
  }
  else if (pars->userinit == 2) {

    seedtype = strrchr(pars->seeds,'.');
    if (!strcmp(".txt", seedtype)) {
      f1 = fopen(pars->seeds,"r");
      contour = GetSimpleContour(f1);
      for(pos=0; pos<contour->length*2; pos += 2) {
        y = ((contour->data)[pos]+1);
        y = ((contour->data)[pos]+1);
        x = ((contour->data)[pos+1]+1);
        bubbles->seeds_array[y*width+x] = 0;
        /*    printf("x = %i and y = %i\n", x,y); */
      }
      DestroySimpleContour(contour);
      fclose(f1);
    }
    else {
      err = ImageIORead(pars->seeds,&thePInfo);
      if (err == readFailure){
        fprintf(stderr,"could not read file %s. Try another one..\n",pars->seeds);
        exit(1);
      }
      PadIm_UCharToUChar(thePInfo.pic,bubbles->seeds_array,(height+2),(width+2));
    }
   pars->userinit = 3; /* bubbles are already initialized */
 
  }
  
  
}











