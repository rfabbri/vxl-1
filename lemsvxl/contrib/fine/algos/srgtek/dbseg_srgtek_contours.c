#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdio.h>
//#include <iiointerface.h>

#include "dbseg_srgtek_contour_routines.h"
#include "dbseg_srgtek_utils.h"


void main(int argc, char **argv)
{
  int *seed_array,*contour_array;
  unsigned char *image_array, *boundary_array;
  PICINFO thePInfo;
  FILE *fopen(), *f1;
  char *image,*boundary_image,*outimage; 
  int x,y, width,height,ii,no_of_seeds,pos,length_of_contour,err,level;
  char out[128]="";
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
  
  err = ImageIORead(image,&thePInfo);
  if (err == readFailure){
    fprintf(stderr,"could not read file %s. Try another one..\n",image);
    exit(1);
  }
  height = thePInfo.h;  
  width = thePInfo.w;

  length_of_contour = height*width;

  seed_array = (int *) malloc(height*width*sizeof(int));
  contour_array = (int *) malloc(height*width*sizeof(int));
  boundary_array = (unsigned char *) malloc(height*width*sizeof(unsigned char));
  
  CopyUByteToInt(thePInfo.pic,seed_array,1,height,width);
  
  err = ImageIORead(boundary_image,&thePInfo);
  if (err == readFailure){
    fprintf(stderr,"could not read file %s. Try another one..\n",boundary_image);
    exit(1);
  }
  CopyUByteToUByte(thePInfo.pic,boundary_array,1,height,width);
  
  find_start(level, contour_array, length_of_contour, seed_array, (width-1), (height-1));
 
  pos=0; 
  while(contour_array[pos] != ENDOFCHAINLIST) {
     while (contour_array[pos] != CHAINEND) {
           x = contour_array[pos]; pos++;
           y = contour_array[pos]; pos++; 
           boundary_array[y*width+x] = 1;
      
      }
    pos++; pos++;
   }


  CopyUByteToUByte(boundary_array,thePInfo.pic,1,height,width);

  err = ImageIOWrite(outimage,&thePInfo,thePInfo.frmType,F_BWDITHER);
  if (err == writeFailure){
    fprintf(stderr,"unable to write out the results to file %s.\nD'oooh!\n",outimage);
    exit(1);
  } /* if err = write .... */




free(seed_array);
free(boundary_array);
free(contour_array);
  
}

                
   


#ifdef __cplusplus
}
#endif





