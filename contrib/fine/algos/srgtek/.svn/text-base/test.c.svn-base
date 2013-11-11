#include <math.h>
#include <stdio.h>
#include <iiointerface.h>

#include "contour_routines.h"
#include "utils.h"


void main(int argc, char **argv)
{
  float *surface_array; 
  int *seed_array,*contour_array;
  unsigned char *image_array, *boundary_array;
  PICINFO thePInfo;
  FILE *fopen(), *f1;
  char *image,*boundary_image,*outimage; 
  char out[128]="";
  int x,y, width,height,ii,no_of_seeds,pos,length_of_contour,err,level,counter;
  struct window
    {
      int x0, y0;
      int x1, y1;
    }my_window;
  
  
  if (argc != 5) {
    fprintf(stderr,"usage: image boundary-image outimage level\n");
    exit(11);
  }

  ii = 1;
  image = argv[ii]; ii++;
  boundary_image = argv[ii]; ii++;
  outimage = argv[ii]; ii++;
  level = atoi(argv[ii]); ii++;
  
  ImageIOInit();
  ImageIOPrintWarnings(False);
  ImageIOPrintVerbose(False); 
  
  err = ImageIORead(boundary_image,&thePInfo);
  if (err == readFailure){
    fprintf(stderr,"could not read file %s. Try another one..\n",image);
    exit(1);
  }
  height = thePInfo.h;  
  width = thePInfo.w;
  my_window.x0 = 0;
  my_window.y0 = 0;
  my_window.x1 = width;
  my_window.y1 = height;

 
  length_of_contour = height*width;
  
  surface_array = (float *) malloc(height*width*sizeof(float));
  seed_array = (int *) malloc(height*width*sizeof(int));
  contour_array = (int *) malloc(height*width*sizeof(int));
  boundary_array = (unsigned char *) malloc(height*width*sizeof(unsigned char));
  
  f1 = fopen(image, "r");
  fread(surface_array, sizeof(float), (height*width), f1);
  fclose(f1);

  pos=0;
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      if (surface_array[pos] > 0.0)
    seed_array[pos] = 0;
      else seed_array[pos] = 1;
      boundary_array[pos] =1;
      pos++;
    }
  }

  find_start(level, contour_array, length_of_contour, seed_array, (width-1), (height-1));
 
  pos=0; counter=1;
  while(contour_array[pos] != ENDOFCHAINLIST) {
     while (contour_array[pos] != CHAINEND) {
           x = contour_array[pos]; pos++;
           y = contour_array[pos]; pos++; 
           boundary_array[y*width+x] = 0;
    
      }
    pos++; pos++; counter++;
     
   }
  
  
/*  fill(1,1,width,my_window,boundary_array,0); */ 
  
#ifdef TEST
  f1=fopen("/en/tek/j2.raw", "w");
  fwrite(boundary_array,sizeof(unsigned char),(height*width),f1);
  fclose(f1);
#endif TEST



  CopyUByteToUByte(boundary_array,thePInfo.pic,1,height,width); 

  err = ImageIOWrite(outimage,&thePInfo,thePInfo.frmType,F_SUNRAS);
  if (err == writeFailure){
    fprintf(stderr,"unable to write out the results to file %s.\nD'oooh!\n",outimage);
    exit(1);
  } /* if err = write .... */




free(seed_array);
free(boundary_array);
free(contour_array);
  
}

                
   







