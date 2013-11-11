#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdio.h>
#include <iiointerface.h>

#include "dbseg_srgtek_contour_routines.h"
#include "dbseg_srgtek_find_contours.h"
#include "dbseg_srgtek_grow.h"
#include "dbseg_srgtek_utils.h"


void main(int argc, char **argv)
{
  int *seed_array,*aux_seed_array,*contour_array;
  unsigned char *image_array, *boundary_array;
  int width,height,ii,no_of_seeds,pos,length_of_contour,err;
  float *mean_averages,*ssl_array;
  PICINFO thePInfo;
  FILE *fopen(), *f1;
  char *image,*seedimage,*outimage; 
  char out[128]="";
  if (argc != 5) {
    fprintf(stderr,"usage: seedimage image outimage no_of_seeds\n");
    exit(11);
  }

  ii = 1;
  seedimage = argv[ii]; ii++;
  image = argv[ii]; ii++;
  outimage = argv[ii]; ii++;
  no_of_seeds = atoi(argv[ii]); ii++;
  
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
  aux_seed_array = (int *) malloc(height*width*sizeof(int));
  contour_array = (int *) malloc(height*width*sizeof(int));
  image_array = (unsigned char *) malloc(height*width*sizeof(unsigned char));
  boundary_array = (unsigned char *) malloc(height*width*sizeof(unsigned char));
  mean_averages = (float *) malloc(3*no_of_seeds*sizeof(float));
  ssl_array = (float *) malloc(4*height*width*sizeof(float));
  
  /* Assigning the image data to image array*/
  CopyUByteToUByte(thePInfo.pic,image_array,1,height,width);
  
  err = ImageIORead(seedimage,&thePInfo);
  if (err == readFailure){
    fprintf(stderr,"could not read file %s. Try another one..\n",seedimage);
    exit(1);
  }
  /* Assigning the seed data to seed array*/
  CopyUByteToInt(thePInfo.pic,seed_array,1,height,width);
  
  ssl_array[0] = 9999;
  ssl_array[1] = 0;
  ssl_array[2] = 0;
  ssl_array[3] = 0;
  ssl_array[4] = -1;

  /*Find the mean of the area in the image under the various seeds
    Arguments:
    seed_array    = int array (height*width) containing the seeds
    image_array   = uchar array (height*width)  containing the image data
    mean_averages = float array (3*no_of_seeds) 
    height        = height of image
    width         = width of image*/

  find_mean_averages(seed_array, image_array, mean_averages, 
             no_of_seeds, height, width);

  for(ii=1; ii<=no_of_seeds; ii++) {
    printf("ii = %i\n", ii);
    
    /*Find the start of a seed region. It calls find_contour
      and find the contour bounding the seed

      Arguments:
      ii                = index runs from 1 to no_of_seeds
      contour_array     = int array (height*width)
      length_of_contour = height*width
      seed_array        = int array (height*width) containing the seeds */
    find_start(ii, contour_array, length_of_contour, 
           seed_array, (width-1), (height-1));
    initial_neighbors(contour_array, seed_array, boundary_array, 
              aux_seed_array,image_array, mean_averages, 
              ssl_array, no_of_seeds, height, width);
  }

  printf("Initialization done \n");

  /* Grows the seeds into full regions.
     Arguments:
     seed_array     = int array (height*width) containing the seeds 
     boundary_array 
     aux_seed_array 
     image_array    = uchar array (height*width)  containing the image data
     ssl_array 
     no_of_seeds 
     mean_averages, 
     height, width
 
  grow_regions(seed_array, boundary_array, aux_seed_array, 
           image_array, ssl_array, 
           no_of_seeds, mean_averages, height, width);
 
  printf("test\n"); 

  CopyIntToUByte(seed_array,thePInfo.pic,1,height,width);

  err = ImageIOWrite(outimage,&thePInfo,thePInfo.frmType,F_SUNRAS);
  if (err == writeFailure){
    fprintf(stderr,"unable to write out the results to file %s.\nD'oooh!\n",outimage);
    exit(1);
  } /* if err = write .... */


#ifdef TEST
pos=0;
while(ssl_array[pos] != -1.0) {
  printf("seed_label = %f\n", ssl_array[pos]);
pos++; 
}

#endif TEST





free(seed_array);
free(boundary_array);
free(aux_seed_array);  
free(contour_array);
free(image_array);
free(mean_averages); 
free(ssl_array);
  
}

                
   


#ifdef __cplusplus
}
#endif





