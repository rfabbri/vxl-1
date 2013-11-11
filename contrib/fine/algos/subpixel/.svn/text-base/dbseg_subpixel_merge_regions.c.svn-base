#ifdef __cplusplus
extern "C" {
#endif

#include "dbseg_subpixel_postscript.h"

#include "dbseg_subpixel_subpixel_bubbles.h"


#define MAX_NO_SHK_GRPS 1000

/***************************************************************
  Description: Top level function that tests the statistics of
               adjacent regions, and decides whether they should
           be merged or not.
  Input : spbub_deform : The Subpixel bubble structure.
  Output: spbub_deform : The Subpixel bubble structure.

***************************************************************/

void merge_all_similar_regions(SP_Bubble_Deform *spbub_deform)
{
 
  /* For all pairs of labels in the region label array. 
     Do the following:
     1. Find the common shock.
     2. Do a CEDT from the shock.
     3. Find the area that intersects a narrow band CEDT and the 
        regions under consideration.
     4. Test the statistics of this intersection.
  */    
  
  char fname[200];
  int fno;
  struct shk_region_ptr{
    int boundary_label1,boundary_label2;
    int shk_grp_id;
  };
  struct shk_region_ptr shk_regn_ptr[MAX_NO_SHK_GRPS];
  int pix_ct[MAX_NO_REGIONS];
  int i,j,x,y,ct,verbose=2;
  int jj,kk,first;
  int reg_count,new_label,new_label_i,new_label_j,merge;
  int height,width;
  int shock_id,no_of_shk_grps,no_of_common_shk_grps,shk_bndry_idx;
  int shk_length;
  int boundary_id1,boundary_id2;
  int *region_label_array, *new_region_label_array, *marked_label_array;
  int shk_group_id_list[50];
  void *data;
  FILE *fp;
  Shocks *shocks;
  Boundary *cedt_boundary;
  Boundary shock_boundary[MAX_SHOCK_LENGTH];

  height=spbub_deform->params.height;
  width=spbub_deform->params.width;
  shocks=&spbub_deform->shocks;
  cedt_boundary=spbub_deform->cedt.boundary;
  marked_label_array=(int *)calloc(height*width,sizeof(int));
  new_region_label_array=(int *)calloc(height*width,sizeof(int));

  /* List all shock groups and the contributing 
     boundary segments for easy reference later*/
  kk=0;jj=0;
  while(shocks->grouplist[kk] != ENDOFSHOCKS) {
    first=1;
    while(shocks->grouplist[kk] != ENDOFSHGROUP) {
      if (shocks->grouplist[kk+1] == ENDOFSHGROUP)
    first=0;
      shock_id=shocks->grouplist[kk];
      if (shocks->id[shock_id].group_id == 39){
    boundary_id1=shocks->id[shock_id].boundary_id[0];
    boundary_id2=shocks->id[shock_id].boundary_id[1];
    printf("Order =%d Bndry_ID1=(%d,%d) Boundary Segments=(%d,%d) \n",
           shocks->id[shock_id].order,
           boundary_id1,boundary_id2,cedt_boundary[boundary_id1].segment,
           cedt_boundary[boundary_id2].segment);
      }
      if (first && shocks->id[shock_id].order != 4){
    shock_id=shocks->grouplist[kk];
    boundary_id1=shocks->id[shock_id].boundary_id[0];
    shk_regn_ptr[jj].boundary_label1=cedt_boundary[boundary_id1].segment;
    boundary_id2=shocks->id[shock_id].boundary_id[1];
    shk_regn_ptr[jj].boundary_label2=cedt_boundary[boundary_id2].segment;
    shk_regn_ptr[jj].shk_grp_id=shocks->id[shock_id].group_id;
    first=0;
    jj++;
      }
      kk++;
    }
    kk++;
  }
  no_of_shk_grps=jj;
  /* Done finding and labelling the no. of shock groups */
  
  if (verbose==2){
    for (i=0;i<no_of_shk_grps;i++){
      printf("Bndry_id1=%d Bndry_id1=%d ShkGrpid=%d\n",
         shk_regn_ptr[i].boundary_label1,shk_regn_ptr[i].boundary_label2,
         shk_regn_ptr[i].shk_grp_id);
    }
  }

  /* Find all the regions that are in the region label array */
  region_label_array=spbub_deform->bubbles.region_label_array;
  for (i=0;i<=MAX_NO_REGIONS-1;i++)
    pix_ct[i]=0;
  for (i=0;i<height*width;i++){
    assert(region_label_array[i] >= 0 && region_label_array[i] < MAX_NO_REGIONS);
    pix_ct[region_label_array[i]]++;
  }
  
  /* For each pair of regions, see if they are adjacent (in that
     case they will have a common shock group); test the statistics
     around a narrow region around the common shock */
  fno=0;
  for (i=1;i<=MAX_NO_REGIONS-1;i++){
    if (pix_ct[i] > 0){
      for (j=i+1;j<=MAX_NO_REGIONS-1;j++){
    if (pix_ct[j] > 0){
      /* printf("Region1=%d Region2=%d \n",i,j); */
      shk_bndry_idx=0;jj=0;
      for (kk=0;kk<no_of_shk_grps;kk++){
        if ( (shk_regn_ptr[kk].boundary_label1==i &&
          shk_regn_ptr[kk].boundary_label2==j) ||
         (shk_regn_ptr[kk].boundary_label1==j &&
          shk_regn_ptr[kk].boundary_label2==i) ){
          /*printf("Bndry_id1=%d Bndry_id1=%d ShkGrpid=%d\n",
        shk_regn_ptr[kk].boundary_label1,
        shk_regn_ptr[kk].boundary_label2,
        shk_regn_ptr[kk].shk_grp_id);*/
          shk_group_id_list[jj]=shk_regn_ptr[kk].shk_grp_id;
          jj++;
        }
      }
      no_of_common_shk_grps=jj;
      /* If there is common shock groups find if the regions
         are to merged */
      if (no_of_common_shk_grps > 0){
        printf("Region1=%d Region2=%d \n",i,j);
        shk_group_id_list[jj]=NOLABEL;
        shk_length=
          convert_selected_boundary_to_linear(spbub_deform->shock_boundary,
                          shock_boundary,
                          shk_group_id_list);
        mark_band_around_common_boundary(shock_boundary,region_label_array,
                         marked_label_array,i,j,height,width);
        printf("Shock length = %d\n",shk_length);
        
        /*sprintf(fname,"reg_shocks-%04d.ps",fno);
          init_draw_ps_original(fname, 10, width,height);fno++;
          draw_gridlines(0,0, width,height, YELLOW, data);
          draw_cedt_boundary_segment_zoom(cedt_boundary,0,0,RED);
          draw_cedt_boundary_segment_zoom(shock_boundary,0,0,PURPLE);
          for (y=0;y<height;y++){
          for (x=0;x<width;x++){
          if (marked_label_array[y*width+x]>0)
          point_draw_ps((double)x,(double)y,RED,data);
          }
          }
          close_draw_ps();*/

        merge=should_regions_be_merged(spbub_deform->image,marked_label_array,
                       i,j,shk_length,height,width);
        if (verbose){
          printf("Merge=%d\n",merge);
        }
        
        update_new_region_label_array(region_label_array, new_region_label_array,
                   i, j, merge, height, width);
        
 
      }/* if (no_of_common_shk_grps > 0) */
    }
      }
    }
  }

  fp=fopen("region.dat","w");
  fwrite(region_label_array,sizeof(int),height*width,fp);
  fclose(fp);
  fp=fopen("new-region.dat","w");
  fwrite(new_region_label_array,sizeof(int),height*width,fp);
  fclose(fp);
  free(new_region_label_array);
  free(marked_label_array);
}

/***************************************************************
  Description: Function that updates the region label array, once
               it is decided whether the regions (represented by
           labels i & j) are to merged or not. If the regions
           are to be merged, they should have the same label 
           in the new region label array.
  Inputs: region_label_array - old region label array.
          i,j - labels of the regions that are considered for merger.
      merge - 0 -> don't merge
              1 -> merge
          height,width - image dimension.

***************************************************************/

void update_new_region_label_array(int *region_label_array, 
                   int *new_region_label_array,
                   int i, int j, int merge, 
                   int height, int width)
{

  int new_label,new_label_i,new_label_j;
  int x,y;


  /* Find whether the labels i,j have been set in the
     new_region_label_array before */
  new_label_i=0;
  new_label_j=0;
  for (y=0;y<height;y++){
    for (x=0;x<width;x++){
      if (region_label_array[y*width+x] == i){
    if (new_region_label_array[y*width+x]>0){
      new_label_i = new_region_label_array[y*width+x];
    }
      }
      if (region_label_array[y*width+x] == j){
    if (new_region_label_array[y*width+x]>0){
      new_label_j = new_region_label_array[y*width+x];
    }
      }
    }
  }

  /* If regions are to be merged, both the labels should be made same 
     in the new_region_label_array.*/
     
  if (merge == 1){
    /* if both labels have been allocated before, choose the smallest one. */
    if (new_label_i > 0 && new_label_j > 0)
      new_label = (new_label_i < new_label_j) ? new_label_i : new_label_j;
    /* if both labels have been unallocated, choose the smallest of i,j */
    else if (new_label_i == 0 && new_label_j == 0) 
      new_label = (i < j) ? i : j;
    /* If only one have been alocated so far, choose the allocated one, 
       which is the non-zero one */
    else
      new_label = (new_label_i > new_label_j) ? new_label_i : new_label_j;
    
    /* Change all pixels that have i or j as their old label, or 
       new_label_i (non-zero) or new_label_i (non-zero) as its new
       label */
    for (y=0;y<height;y++){
      for (x=0;x<width;x++){
    if (region_label_array[y*width+x] == i ||
        region_label_array[y*width+x] == j ||
        (new_label_i != 0 && 
         new_region_label_array[y*width+x] == new_label_i) ||
        (new_label_j != 0 && 
         new_region_label_array[y*width+x] == new_label_j) ){
      new_region_label_array[y*width+x]=new_label;
    }
      }
    }
  }/* if (merge == 1)*/

  /* if not merging copy the labels if new label is zero */
  else{
    if (new_label_i > 0){
      /*Do nothing */
    }
    else{
      for (y=0;y<height;y++){
    for (x=0;x<width;x++){
      if (region_label_array[y*width+x] == i)
        new_region_label_array[y*width+x]=i;
    }
      }
    }
    if (new_label_j > 0){
      /*Do nothing */
    }
    else{
      for (y=0;y<height;y++){
    for (x=0;x<width;x++){
      if (region_label_array[y*width+x] == j)
        new_region_label_array[y*width+x]=j;
    }
      }
    }
  }/*else of if(merge == 1)*/
  

}

/******************************************************************
  Description: Function that decides whether two regions should be
               merged or not.
  Inputs: image - image array.
          marked_label_array - The array to flag all the pixels that
              belong to either of the two regions that  lie 
          within a narrow  around the common boundary.
      region_label1/2 - the labels of the regions that are 
              considered for merger.
          bndry_length - length of common boundary.
      height,width - image dimensions.
  Return: 1 -> if regions are to merged.
          0 -> if regions are not to merged.
*******************************************************************/


int should_regions_be_merged(double *image, int *marked_label_array, 
                 int region_label1,int region_label2, int bndry_length,
                 int height, int width)
{
  Region_Stats region1, region2;
  Region_Stats region12;
  FILE *fp;
  double n1,n2,mu1,mu2,s1,s2,n12,mu12,s12;
  double dE;
  

  /* Find the statistcs of the "marked" portions of the regions.*/
  find_mean_stdev_region(image,marked_label_array,region_label1,
                height,width,&region1);
  find_mean_stdev_region(image,marked_label_array,region_label2,
                height,width,&region2);

  n1=region1.no_of_pixels;
  n2=region2.no_of_pixels;
  mu1=region1.mean;
  mu2=region2.mean;
  s1=region1.stdev;
  s2=region2.stdev;
  
  if (n1 == 0 || n2 == 0)
    return 0;
  
  /* Computing the statistics of the merged region */
  region12.no_of_pixels=(n1+n2);
  n12=region12.no_of_pixels;
  region12.mean=(n1*mu1+n2*mu2)/(n1+n2);
  mu12=region12.mean;
  s12=sqrt((n1*pow(s1,2.0)+n2*pow(s2,2.0)+(n1*n2/(n1+n2))*pow(mu1-mu2,2.0))/(n1+n2));
  region12.stdev=s12;

  /* Change in energy. Formula from Zhu/Yuille's paper */
  dE=n1*log((s12*s12)/(s1*s1)) + n2*log((s12*s12)/(s2*s2)) + 1;

  fp=fopen("reg-stats.dat","a");
  fprintf(fp,"Region1=%d Region2=%d \n",region_label1,region_label2);
  /* print_region_stats(&region1,fp); 
  print_region_stats(&region2,fp);
  print_region_stats(&region12,fp); */
  fprintf(fp,"N(%f,%f) #%d \n",region1.mean,region1.stdev,region1.no_of_pixels);
  fprintf(fp,"N(%f,%f) #%d \n",region2.mean,region2.stdev,region2.no_of_pixels);
  fprintf(fp,"N(%f,%f) #%d \n",region12.mean,region12.stdev,region12.no_of_pixels);
  fprintf(fp,"L=%d ProbdE = %f dE = %f \n",bndry_length,dE,0.5*dE-1*bndry_length);
  fprintf(fp,"\n");
  fclose(fp);

  /* Merging condition. (from Zhu/Yuille's paper) */
  if ((0.5*dE-1*bndry_length) < 5){
    return 1;
  }
  else{
    return 0;
  }


}

/******************************************************************
  Description: Function that marks the band around the common 
               boundary of the two regions. This determines the
           pixels that are used to compute the statistics
           that decide whether two regions should merge or
           not.
  Inputs: boundary - The common boundary between the two regions.
          region_label_array - Array of region labels.
      region_label1/2 - the labels of the regions that are 
              considered for merger.
      height,width - image dimensions.
  Output: marked_label_array - The array to flag all the pixels that
              belong to either of the two regions that  lie 
          within a narrow  around the common boundary.
*******************************************************************/
      

void mark_band_around_common_boundary(Boundary *boundary, int *region_label_array,
                      int *marked_label_array, int region_label1,
                      int region_label2, int height, int width)
{
  SPCedt cedt;
  CEDTWaves waves;
  Heap heap;
  int heap_size;
  int i,x,y;

  allocate_data_SP_Cedt(&cedt, height, width);
  cedt.boundary_end = 0;
  cedt.boundary_start = 0;
  cedt.boundary = boundary;

  heap_size = height*width+5*MAX_BOUNDARY_LENGTH;
  alloc_mem_for_heap_SPCedt(&heap,heap_size);
  initialize_heap_SPCedt(&heap, heap_size);

  cedt.ray=(Ray *)calloc(5*MAX_BOUNDARY_LENGTH,sizeof(Ray));
  waves.qwaves=(QWaves *)calloc(height*width,sizeof(QWaves)); 
  waves.qorder=(QuenchOrder *)calloc(height*width,sizeof(QuenchOrder));
  waves.inter_boundary = 1;
  for (i=0;i<height*width;i++)
    waves.qwaves[i].quench = NULL;

  free_data_QWaves_list(waves.qwaves,height*width);
  initial_wave_prop_cedt_strucs(&heap,&cedt,&waves,height,width);
  wave_propagation_via_cedt(&heap, &cedt, &waves, height, width);

  for (y=0;y<height;y++){
    for (x=0;x<width;x++){
      marked_label_array[y*width+x]=0;
    }
  }

  printf("Region labels = (%d,%d)\n",region_label1,region_label2);

  for (y=0;y<height;y++){
    for (x=0;x<width;x++){
      if ( (abs(cedt.surface[y*width+x]) < 25) &&
       (region_label_array[y*width+x] == region_label1) ){
    marked_label_array[y*width+x]=region_label1;
      }
      else if ( (abs(cedt.surface[y*width+x]) < 25) &&
        (region_label_array[y*width+x] == region_label2) ){
    marked_label_array[y*width+x]=region_label2;
      }
    }
  }
  
  free_data_SP_Cedt(&cedt);
  free_mem_heap_SP_Cedt(&heap);
  free(cedt.ray);
  free_data_QWaves_list(waves.qwaves,height*width);
  free(waves.qwaves);
  free(waves.qorder);

}

#ifdef __cplusplus
}
#endif





