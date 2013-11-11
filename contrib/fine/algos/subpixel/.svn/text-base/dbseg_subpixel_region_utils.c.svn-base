#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "dbseg_subpixel_statistics.h"

#include "dbseg_subpixel_subpixel_bubbles.h"

/********************************************************************
  Decsription:  Function that computes the mean and standard deviation   
                of theregions that are labelled in the image
  Inputs :    image_array - Image array    
              seed_array - Array of region labels
          region_flag - Label of region for which stats is found
          height -  Height of image
          width -  Width of image
  Output :    region Structure having all region information

  ******************************************************************/
void find_mean_stdev_region(double *image_array, int *seed_array,  
                int region_flag, int height,   
                int width, Region_Stats *region)
{


  int i,j;
  double total=0,std_total=0,temp=0;
  region->no_of_pixels=0;
  
  /*Set the region->region_label to the label of the region under consideration*/
  region->region_label=region_flag;

  /*   Loop through to find the no. of elements and the */
  /*   total intensity of each region */
  for(i=0;i<height;i++){
    for(j=0;j<width;j++){
      if (seed_array[i*width+j] == region_flag){
    total=total+image_array[i*width+j];
    region->no_of_pixels++;
      }
    }
  }
  
  /*   Find the MEAN */
  region->sum=total;
  if (region->no_of_pixels != 0)
    region->mean=total/region->no_of_pixels;
  else
    region->mean=0.0;
  
  /*   Loop through to find the no. of elements and the */
  /*   standard deviation of each region */
  for(i=0;i<height;i++){
    for(j=0;j<width;j++){
      if (seed_array[i*width+j] == region_flag){
    temp=image_array[i*width+j]-region->mean;
    std_total=std_total+temp*temp;
      }
    }
  }
  
  /*   Find the STANDARD DEVIATION */
  if (region->no_of_pixels != 0)
    region->stdev=sqrt(std_total/region->no_of_pixels);
  else
    region->stdev=sqrt(std_total);
  
}/*End of function find_mean_stdev_region*/

/********************************************************************
  Decsription:  Function that computes the mean and standard deviation   
                of theregions that are labelled in the image
  Inputs :    image_array - Image array    
              seed_array - Array of region labels
          region_flag - Label of region for which stats is found
          height -  Height of image
          width -  Width of image
  Output :    region Structure having all region information

  ******************************************************************/
void find_em_multi_mean_stdev_region(double *image_array, int *seed_array,  
                     int region_flag, int height,   
                     int width, Region_Stats *region)
{


  int i,j;
  double total=0,std_total=0,temp=0;
  double *arr;
  GaussParameter par;
  FILE *fp;
  
  
  region->no_of_pixels=0;
  initializeGaussParameter(&par,2);
  arr=(double*)calloc(sizeof(double),height*width);


  /*Set the region->region_label to the label of the region under consideration*/
  region->region_label=region_flag;

  /*   Loop through to find the no. of elements and the */
  /*   total intensity of each region */
  for(i=0;i<height;i++){
    for(j=0;j<width;j++){
      if (seed_array[i*width+j] == region_flag){
    arr[region->no_of_pixels]=image_array[i*width+j];
    region->no_of_pixels++;
      }
    }
  }
  
  par.mean[0]=region->mean_vec[0];
  par.sigma[0]=region->std_vec[0];
  par.alpha[0]=region->alpha_vec[0];
  par.mean[1]=region->mean_vec[1];
  par.sigma[1]=region->std_vec[1];
  par.alpha[1]=region->alpha_vec[1];

  fp=fopen("em.txt","a");
  fprintf(fp,"Region-label=%d\n",region_flag);
  fprintf(fp,"%5.2f N(%5.2f,%5.2f) + %5.2f N(%5.2f,%5.2f)\n",par.alpha[0],par.mean[0],
     par.sigma[0],par.alpha[1],par.mean[1],par.sigma[1]);
  fclose(fp);
  gaussMixtureParameterEstimationByEM(arr, region->no_of_pixels, &par);
  /*printf("%5.2f N(%5.2f,%5.2f) + %5.2f N(%5.2f,%5.2f)\n",par.mean[0],
    par.sigma[0],par.alpha[0],par.mean[1],par.sigma[1],par.alpha[1]);*/
  
  region->mean_vec[0]=par.mean[0];
  region->std_vec[0]=par.sigma[0];
  region->alpha_vec[0]=par.alpha[0];
  region->mean_vec[1]=par.mean[1];
  region->std_vec[1]=par.sigma[1];
  region->alpha_vec[1]=par.alpha[1];

  region->mean=region->mean_vec[0];
  region->stdev=region->std_vec[0];
  freeGaussParameters(&par);
  free(arr);
  
}/*End of function find_mean_stdev_region*/


/********************************************************************
  Decsription:  Function that computes the Gaussian probability that
                the intensity belongs to a particular region.
  Inputs :    intensity - Intensity value of interest
              region_flag - Label of region for which mean is found
              region region - Structure having all region information
          no_of_regions - total number of regions in the image
  Returns :  Gaussian probability that the intensity belongs to the 
             region specified by region_flag
          
  ******************************************************************/

double find_statistics_force(double intensity, Region_Stats *region,
                int region_flag,int no_of_regions)
{
  double force;
  int region_index;

  if (region_flag > no_of_regions){
    printf("Invalid region in find_statistics_force ");
    printf("Region=%d No. of regions = %d\n",region_flag, no_of_regions);
    assert(region_flag <= no_of_regions);
  }

  region_index=region_flag-1;
  force = (intensity-region[region_index].mean)/region[region_index].stdev;
  return force;
}


/********************************************************************
  Decsription:  Function that computes the Gaussian probability that
                the intensity belongs to a particular region. Here 
        instead of a single pixel, a window is considered.
  Inputs :    window - Window_stats for the image
              x,y - location of the pixel
          height, width - image dimensions
              region_flag - Label of region for which mean is found
              region region - Structure having all region information
          no_of_regions - total number of regions in the image
  Returns :  Gaussian probability that the intensity belongs to the 
             region specified by region_flag for a window around the
         selected pixel
          
  ******************************************************************/


double find_statistics_force_window(Window_Stats *win, int x, int y,
                   int height, int width, Region_Stats *region,
                   int region_flag,int no_of_regions)
{
  double F1, F2, F3, F;
  int region_index,pos;

  assert(region_flag <= no_of_regions);
  assert(x>=0 && x<height && y>=0 && y<width);

  pos=y*width+x;
  region_index=region_flag-1;
  F1=log(2*M_PI*pow(region[region_index].stdev,2));
  F2=pow(((win->mean[pos]-region[region_index].mean) / region[region_index].stdev),2);
  F3=pow((win->stdev[pos]/region[region_index].stdev),2);
  F=F1+F2+F3;
  return F;
}

/*******************************************************************
  Decsription: Function to compute the mean and standard deviation 
               for a square window around a pixel and  save the 
           results for every pixel in the image.
  Inputs : image_array   Image
           height,width - image dimensions
       window_size  Size of window used

  Output : win  -  Structure for storing mean & stdev

*******************************************************************/

void compute_square_window_stats(double *image_array,Window_Stats *win, 
                int height, int width, int window_size)
{
  int i,j;
  int k,l;
  int no_of_pixels=0;
  double mean_total,std_total,stdev;
  int sz=(window_size-1)/2;
  double temp;

  assert(window_size >= 2);

  /*Find the mean and no_of_pixels for each pixel*/
  for (i=0;i<height;i++){
    for (j=0;j<width;j++){
      no_of_pixels=0;
      mean_total=0.0;
    
      for (k=imax(i-sz,0);k<=imin(i+sz,height-1);k++){
    for (l=imax(j-sz,0);l<=imin(j+sz,width-1);l++){
      no_of_pixels++;
      mean_total=mean_total+image_array[k*width+l];
    }
      }
      win->mean[i*width+j]=mean_total/no_of_pixels;
      win->no_of_pixels[i*width+j]=no_of_pixels;
    }
  }

  /*Find the standard deviation for each pixel*/
  for (i=0;i<height;i++){
    for (j=0;j<width;j++){
      std_total=0.0;
    
      for (k=imax(i-sz,0);k<=imin(i+sz,height-1);k++){
    for (l=imax(j-sz,0);l<=imin(j+sz,width-1);l++){
      temp=image_array[k*width+l]-win->mean[i*width+j];
      std_total=std_total+temp*temp;
    }
      }
      if (win->no_of_pixels[i*width+j] != 0)
    win->stdev[i*width+j]=sqrt(std_total/win->no_of_pixels[i*width+j]);
      else
    win->stdev[i*width+j]=0.0;
    }
  }

}/*End of function compute_square_window_stats*/ 

/*******************************************************************
  Decsription: Function to print all the information of region.
  Inputs : region Region_Stat stucture to be printed

*******************************************************************/

void print_region_stats(Region_Stats *region,FILE *fp)
{
  fprintf(fp,"Region Label=%d\n",region->region_label);
  fprintf(fp,"Mean=%f Std=%f\n",region->mean,region->stdev);
  fprintf(fp,"Sum=%f No.=%d\n",region->sum,region->no_of_pixels);
  /* fprintf(fp,"\n"); */
}


/*******************************************************************
  Decsription: Find the no. of labels in the inputs seed image.
  Input : seed - seed array
  Return: no. of regions.
*******************************************************************/
int find_regions(int *region_label_array,int height,int width)
{
  int reg_count=0,pix_count[256];
  int i;
  
  for (i=0;i<255;i++)
    pix_count[i]=0;
  
  for (i=0;i<height*width;i++)
    pix_count[(int)region_label_array[i]]++;
  
  for (i=1;i<255;i++)
    if (pix_count[i] > 0){
      printf("Region: %d  Count %d\n",i,pix_count[i]);
      reg_count++;
    }
  printf("No. of Regions: %d\n",reg_count);

  return reg_count;
}

/*******************************************************************
  Decsription: Function to update all the region information .
  Inputs : spbub_deform SP_Bubble_Deform structure

*******************************************************************/
void update_region_structure(SP_Bubble_Deform *spbub_deform)
{
  int i;
  int x,y,pos;
  int boundary_id, contour_id;
  int height,width;

  height=spbub_deform->params.height;
  width=spbub_deform->params.width;

  /*spbub_deform->bubbles.no_of_regions = 
    fill_regions(spbub_deform->enosurface,spbub_deform->bubbles.region_label_array,
    spbub_deform->bubbles.surface);*/
  
  /*update the region label array based on the contour id of the boundary label
    that minimizes the distance at this point */
  for (y=PAD;y<height-PAD;y++){
    for (x=PAD;x<width-PAD;x++){
      pos=y*width+x;
      if ((spbub_deform->cedt.tag[pos] == 0 || spbub_deform->cedt.tag[pos] == 1) ||
      (spbub_deform->bubbles.surface[pos] < 1.0)){
    if (spbub_deform->bubbles.surface[pos] > 0){
      boundary_id=spbub_deform->cedt.label[pos];
      contour_id=spbub_deform->cedt.boundary[boundary_id].segment;
      spbub_deform->bubbles.region_label_array[pos]=contour_id;
    }
    else{
      spbub_deform->bubbles.region_label_array[pos]=0;
    }
      }
    }
  }
  adjust_boundaries_pad_int(spbub_deform->bubbles.region_label_array,height,width,PAD);

  /*for (i=0;i<spbub_deform->bubbles.no_of_regions;i++){
    find_mean_stdev_region(spbub_deform->image,
    spbub_deform->bubbles.region_label_array, 
    i+1, height,width,
    &spbub_deform->bubbles.region[i]);
    }*/
  spbub_deform->bubbles.no_of_regions=
    update_region_stats(spbub_deform->bubbles.region_label_array, 
            spbub_deform->image, spbub_deform->bubbles.region,
            spbub_deform->params.no_of_classes,height,width);
  
}


/********************************************************************
  Decsription:  Function that finds the different labels in the 
                region_label_array and computes the mean and standard 
        deviation for each of them.
  Inputs :    image_array - Image array    
              region_label_array - Array of region labels
          no_of_classes - no of classes in multi gaussian 
                          approximation of each seed.
          height -  Height of image
          width -  Width of image
  Output :    region Structure having all region information
  
******************************************************************/
int update_region_stats(int *region_label_array, double *image, 
            Region_Stats *region,int no_of_classes, 
            int height, int width)
{
  int pix_ct[MAX_NO_REGIONS];
  int i,ct,verbose=0;
  int reg_count;


  for (i=0;i<=MAX_NO_REGIONS-1;i++)
    pix_ct[i]=0;
  for (i=0;i<height*width;i++){
    if (region_label_array[i] < 0 || 
    region_label_array[i] >= MAX_NO_REGIONS){
      printf("%d Reg_label=%d \n",i,region_label_array[i]);
    }
    assert(region_label_array[i] >= 0 && 
       region_label_array[i] < MAX_NO_REGIONS);
    pix_ct[region_label_array[i]]++;
  }

  reg_count=0;
  for (i=1;i<=MAX_NO_REGIONS-1;i++){
    if (pix_ct[i] > 0){
      if (no_of_classes == 1){
    find_mean_stdev_region(image,region_label_array,i, height,width,
                   &region[i]);
      }
      else if (no_of_classes == 2){
    printf("Doing parameter estimation via EM\n");
    find_em_multi_mean_stdev_region(image,region_label_array,i, height,width,
                    &region[i]);
      }
      if (verbose){
    printf("Region Label=%d No.=%d\n",i,pix_ct[i]);
    print_region_stats(&region[i], stdout);
      }
      reg_count++;
    }
    else{
      region[i].no_of_pixels=0;
    }
  }
  return reg_count;
}

#ifdef __cplusplus
}
#endif





