#include <stdio.h>
#include <vcl_cmath.h>

#include "utils.h"

#include "euclidean_dist.h"

//: \param[in] direction    indicates if internal or external DT is to be
// computed. If direction == 1, then distances are computed on the 1-pixels,
// while the distance tranform at a 0-pixel is 0. If direction == 0, then
// distances are computed on the 0-pixels, while the DT at a 1-pixel is 0.
// This is totally unecessary, as the user could simply invert the input image
// to get an DT in the complementary direction. 
//
// \param[out] dt_array     squared distances at each pixel
void 
euclidean(unsigned char *image_array,float *dt_array, int height, int width,int direction)

{
 float *dt_x_array, *dt_y_array;
 FILE *fopen(), *f1;
 int y,x,xx,position;
 float val1x,val1y,val1,val2x,val2y,val2,val3x,val3y,val3,val4x,val4y,val4,val5x,val5y,val5,
        value,level_set_value;
 

  dt_x_array = (float *) malloc (height*width*sizeof(float));
  dt_y_array = (float *) malloc (height*width*sizeof(float));


  // initializes dt_x_array and dt_y_arrays to infinity where distances are to
  // be computed, and to zero at source pixels. Source pixels are 1 if
  // direction==0, or 1 if direction ==0.
  create_distance_images(image_array,dt_x_array,dt_y_array,height,width,direction);
  
  // cache the values of dt_x_array[i]^2 + dt_y_array[i]^2 into dt_array
  compute_euclidean_distance_array_squared(dt_x_array,dt_y_array,dt_array,height,width);

/******************* FIRST SCAN FROM TOP  **************************/

 for(y=0; y<height; y++) {
/* First pass from upper left corner */
   for(x=0; x<width; x++) {
      position = width*y+x;
      val1x = image_element_substitute(x,     y,   dt_x_array,height,width);
      val1y = image_element_substitute(x,     y,   dt_y_array,height,width);
      val1 = sqrt(val1x*val1x+val1y*val1y);
      val2x = image_element_substitute((x-1), y,   dt_x_array,height,width)+1.0;
      val2y = image_element_substitute((x-1), y,   dt_y_array,height,width);
      val2  = sqrt(val2x*val2x+val2y*val2y);
      val3x = image_element_substitute((x-1),(y-1),dt_x_array,height,width)+1.0;
      val3y = image_element_substitute((x-1),(y-1),dt_y_array,height,width)+1.0;
      val3  = sqrt(val3x*val3x+val3y*val3y);
      val4x = image_element_substitute(x,    (y-1),dt_x_array,height,width);
      val4y = image_element_substitute(x,    (y-1),dt_y_array,height,width)+1.0;
      val4 = sqrt(val4x*val4x+val4y*val4y);
      val5x = image_element_substitute((x+1),(y-1),dt_x_array,height,width)+1.0;
      val5y = image_element_substitute((x+1),(y-1),dt_y_array,height,width)+1.0;
      val5 = sqrt(val5x*val5x+val5y*val5y);
      value = min_of_5(val1,val2,val3,val4,val5);
            
      /////printf("%7.1f ",value);

      if (value == val1) {
  dt_x_array[position] = val1x;
  dt_y_array[position] = val1y;
      }
      else if (value == val2) {
  dt_x_array[position] = val2x;
  dt_y_array[position] = val2y;
      }
      else if (value == val3) {
  dt_x_array[position] = val3x;
  dt_y_array[position] = val3y;
      }
      else if (value == val4) {
  dt_x_array[position] = val4x;
  dt_y_array[position] = val4y;
      }
      else if (value == val5) {
  dt_x_array[position] = val5x;
  dt_y_array[position] = val5y;
      }
    } /* for(x=0; x<width; x++)  */
  
/* second pass from right upper corner */
    for(xx=(width-1); xx>=0; xx--) {
       position = width*y+xx;
       val1x = image_element_substitute(xx,y,dt_x_array,height,width);
       val1y = image_element_substitute(xx,y,dt_y_array,height,width);
       val1 = sqrt(val1x*val1x+val1y*val1y);
       val2x = image_element_substitute((xx+1),y,dt_x_array,height,width)+1.0;
       val2y = image_element_substitute((xx+1),y,dt_y_array,height,width);       
       val2 = sqrt(val2x*val2x+val2y*val2y);
       if (val1 > val2) {
          dt_x_array[position] = val2x;
          dt_y_array[position] = val2y;
        }
     } /* for(xx=width-1; .... */
   } /* for(y=0; y<height; y++) */

     
/*******************   SECOND SCAN FROM BOTTOM   ***************************************/

 for(y=(height-1); y>=0; y--) {
   for(x=(width-1); x>=0; x--) {
      position = width*y+x;
      val1x = image_element_substitute(x,     y,   dt_x_array,height,width);
      val1y = image_element_substitute(x,     y,   dt_y_array,height,width);
      val1 = sqrt(val1x*val1x+val1y*val1y);
      val2x = image_element_substitute((x+1), y,   dt_x_array,height,width)+1.0;
      val2y = image_element_substitute((x+1), y,   dt_y_array,height,width);
      val2  = sqrt(val2x*val2x+val2y*val2y);
      val3x = image_element_substitute((x+1),(y+1),dt_x_array,height,width)+1.0;
      val3y = image_element_substitute((x+1),(y+1),dt_y_array,height,width)+1.0;
      val3  = sqrt(val3x*val3x+val3y*val3y);
      val4x = image_element_substitute(x,    (y+1),dt_x_array,height,width);
      val4y = image_element_substitute(x,    (y+1),dt_y_array,height,width)+1.0;
      val4 = sqrt(val4x*val4x+val4y*val4y);
      val5x = image_element_substitute((x-1),(y+1),dt_x_array,height,width)+1.0;
      val5y = image_element_substitute((x-1),(y+1),dt_y_array,height,width)+1.0;
      val5 = sqrt(val5x*val5x+val5y*val5y);
      value = min_of_5(val1,val2,val3,val4,val5);
      
      if (value == val1) {
  dt_x_array[position] = val1x;
  dt_y_array[position] = val1y;
      }
      else if (value == val2) {
  dt_x_array[position] = val2x;
  dt_y_array[position] = val2y;
      }
      else if (value == val3) {
  dt_x_array[position] = val3x;
  dt_y_array[position] = val3y;
      }
      else if (value == val4) {
  dt_x_array[position] = val4x;
  dt_y_array[position] = val4y;
      }
      else if (value == val5) {
  dt_x_array[position] = val5x;
  dt_y_array[position] = val5y;
      }
    } /* for(x=0; x<width; x++)  */
      

/* second pass from right lower corner */
    for(xx=0; xx< width; xx++) {
       position = width*y+xx;
       val1x = image_element_substitute(xx,y,dt_x_array,height,width);
       val1y = image_element_substitute(xx,y,dt_y_array,height,width);
       val1 = sqrt(val1x*val1x+val1y*val1y);
       val2x = image_element_substitute((xx-1),y,dt_x_array,height,width)+1.0;
       val2y = image_element_substitute((xx-1),y,dt_y_array,height,width);
       val2 = sqrt(val2x*val2x+val2y*val2y);
       if (val1 > val2) {
          dt_x_array[position] = val2x;
          dt_y_array[position] = val2y;
        }
     } /* for(xx=width-1; .... */
 } /* for(y=0; y<height; y++) */


 // simply dt_array[i] = dt_x_array[i]^2 + dt_y_array[i]^2
 compute_euclidean_distance_array_squared(dt_x_array,dt_y_array,dt_array,height,width);
 

 free((char *) dt_y_array);
 free((char *) dt_x_array);
}

     
//: 
// \param[in] image binary image (0-1)
// \param[in] distance_from_interior
// \param[out] dt_x
// \param[out] dt_y
void create_distance_images(unsigned char *image,float *dt_x,float *dt_y,int height, int width,
                            int distance_from_interior)
     
 
{
  int xx, yy, pos;
  float large_number; 
  large_number = 99999.0;

  if (distance_from_interior == 0) {  /* set non-zero's to large */
    for(yy=0; yy <height; yy++){
      for(xx = 0; xx <width; xx++){
  pos = yy*width+xx;
        if (image[pos] != 1) {
    dt_x[pos] = large_number;
    dt_y[pos] = large_number;
  }
  else {
    dt_x[pos] = 0.0;
    dt_y[pos] = 0.0;
  } 
      }
    }
  }         
  
  else {                             /* set non-zero's to zero */
    for(yy=0; yy <height; yy++){
      for(xx = 0; xx <width; xx++) {
  pos = yy*width+xx;
  if (image[pos] != 1) {
    dt_x[pos] = 0.0;
    dt_y[pos] = 0.0;
  }
         else {
     dt_x[pos] = large_number;
     dt_y[pos] = large_number;
    } 
      }
    }
 }  

}


void compute_euclidean_distance_array_squared(float *array_x, float *array_y,
                                              float *array,int height, int width)
     
  
{
  int xx, yy, pos;
  float value;

  for(yy=0; yy <height; yy++){
    for(xx = 0; xx <width; xx++) {
      pos = yy*width+xx;
      value = array_x[pos]*array_x[pos] + array_y[pos]*array_y[pos];
      array[pos] = value;
    }
  }
}
