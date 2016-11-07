#include <math.h>
#include <stdio.h>


#include "utils.h"


void allocate_cedt_data(Cedt *cedt, int height,int width) 
     
{
  cedt->dx = (float *) calloc(height*width,sizeof(float));
  cedt->dy = (float *) calloc(height*width,sizeof(float));
  cedt->dir = (int *)  calloc(height*width,sizeof(int));
}  


void free_cedt_data(Cedt *cedt) 

{
  free(cedt->dx);
  free(cedt->dy);
  free(cedt->dir);
  
}








