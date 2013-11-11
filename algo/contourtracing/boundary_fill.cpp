#include "1d_eno_types.h"

/**************************************************************************
  Description: Fill regions based on ENO zerox. The different regions are
               represented by the surface, hence eno zerox represent the
         boundary of each region.
  Input : x,y - current location in image.
          enoimage - ENO interpolated surface.
    region_label - the label of the current region.
  Output: region_label_array - the array of region labels.
**************************************************************************/
void eno_zerox_boundary_fill(int x, int y, ENO_Interval_Image_t *enoimage, 
           int region_label, int *region_label_array)
{
  int height,width ;

  height = enoimage->height;
  width = enoimage->width;

  if (y<2 || y>=height-2 || x<2 || x>=width-2 || 
      region_label_array[y*width+x] == region_label)
    return;

  /* printf("(%d,%d)\n",x,y); */
  region_label_array[y*width+x] = region_label;
  
  /*Look in the forward y-direction (UP)*/ 
  if (y+1 < height && enoimage->vert[y*width+x].zerox == NULL &&
      region_label_array[(y+1)*width+x] != region_label)
    eno_zerox_boundary_fill(x, y+1,enoimage,region_label,region_label_array);

  /*Look in the backward y-direction (DOWN)*/ 
  if (y-1 >= 0 && enoimage->vert[(y-1)*width+x].zerox == NULL &&
      region_label_array[(y-1)*width+x] != region_label)
    eno_zerox_boundary_fill(x, y-1,enoimage,region_label,region_label_array);

  /*Look in the forward x-direction (RIGHT)*/ 
  if (x+1 < width && enoimage->horiz[y*width+x].zerox == NULL &&
      region_label_array[y*width+x+1] != region_label)
    eno_zerox_boundary_fill(x+1, y,enoimage,region_label,region_label_array);

  /*Look in the backward x-direction (LEFT)*/ 
  if (x-1 >= 0 && enoimage->horiz[y*width+x-1].zerox == NULL &&
      region_label_array[y*width+x-1] != region_label)
    eno_zerox_boundary_fill(x-1, y,enoimage,region_label,region_label_array);
}

/**************************************************************************
  Description: Top level function that fill regions based on ENO zerox. 
               It calls the eno_zerox_boundary_fill for each positive
         surface point.
  Input : enoimage - ENO interpolated surface.
    surface - surface array.
  Output: region_label_array - the array of region labels.
  Return: no. of different regions.
**************************************************************************/
int fill_regions(ENO_Interval_Image_t *enoimage, int *region_label_array,
      double *surface)
{
  int height,width ;
  int x,y;
  int region_label;
  FILE *fp;

  height = enoimage->height;
  width = enoimage->width;
  
  for (y=0;y<height;y++){
    for (x=0;x<width;x++){
      region_label_array[y*width+x] = 0;
    }
  }
  region_label = 1;
  for (x=2;x<width-2;x++){
    for (y=2;y<height-2;y++){
      /*Positive surface indicates inside of a region. Region Label array = 0 
  means that the pixel is not yet allocated to any region yet */
      if (surface[y*width+x] > 0.0 && region_label_array[y*width+x] ==0){
  eno_zerox_boundary_fill(x,y,enoimage,region_label,region_label_array);
  region_label++;
      }
    }
  }
  
  adjust_boundaries_pad_int(region_label_array,height,width,2);
      
  

  return region_label-1;
}

void adjust_boundaries_pad_int(int *image, int height, int width, int pad) 
{
  int x,y,pos;

  /* Top left corner (image)*/
  for (y=0;y<pad;y++){
    for (x=0;x<pad;x++){
      image[y*width+x] = image[pad*width+pad];
    }
  }
  
  /* Top right corner (image)*/
  for (y=height-pad;y<height;y++){
    for (x=0;x<pad;x++){
      image[y*width+x] = image[(height-pad-1)*width+pad];
    }
  }

  /* Bottom left corner (image)*/
  for (y=0;y<pad;y++){
    for (x=width-pad;x<width;x++){
      image[y*width+x] = image[pad*width+width-pad-1];
    }
  }
  /* Bottom right corner (image)*/
  for (y=height-pad;y<height;y++){
    for (x=width-pad;x<width;x++){
      image[y*width+x] = image[(height-pad-1)*width+width-pad-1];
    }
  }
  
  /* left and right side */
  for(y=pad; y<height-pad; y++) {
    for (x=0;x<pad;x++){
      image[y*width+x] =  image[y*width+pad];
      image[y*width+(width-x-1)] =  image[y*width+(width-pad-1)];
    }
  }
  /* top and bottom sides */ 
  for(x=pad; x<width-pad; x++) {
    for (y=0;y<pad;y++){
      image[y*width+x] =  image[pad*width+x];
      image[(height-y-1)*width+x] =  image[(height-pad-1)*width+x];
    }
  }
}
