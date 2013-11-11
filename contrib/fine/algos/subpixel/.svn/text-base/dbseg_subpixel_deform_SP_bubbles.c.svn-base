#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dbseg_subpixel_subpixel_bubbles.h"

/**************************************************************************
  Description: Top-level Function that does the subpixel bubble evolution.
  Inputs :  spbub_deform - The SP_Bubble_Deform structure.
  Output :  spbub_deform 
 **************************************************************************/

void subpixel_bubble_evolution(SP_Bubble_Deform *spbub_deform)
{
  Tracer Tr;
  FILE *fp,*time_fp;
  unsigned char *empty_image;
  char reg_fname[200];
  int i,j;
  int height,width;
  int start_from;
  int total_no_iterations;
  int verbose=0,silent=0;
  double *dsurface;

  height = spbub_deform->params.height;
  width = spbub_deform->params.width;
  dsurface = (double *)calloc(height*width,sizeof(double));
  empty_image = (unsigned char *)calloc(height*width,sizeof(unsigned char));

  
  start_from = spbub_deform->params.start_from;
  total_no_iterations = spbub_deform->params.total_no_iterations;
  spbub_deform->params.iteration_no = start_from;

  /* Main Loop of iterations */
  for (i=start_from;i<start_from+total_no_iterations;i++){
    if (!silent){
      printf("Iteration No. %d \n",i);
      fflush(stdout);
    }


    spbub_deform->params.iteration_no++;
    compute_forces(spbub_deform);


    deform_SP_bubbles(spbub_deform);
    /*Trace the ZEROX of deformed surface */
    CopyIm_DoubleToDouble(spbub_deform->bubbles.surface,dsurface,height,width);
    /*Update the region information*/
    if (!silent){
      printf("Updating region structure\n");
      fflush(stdout);
    }
    update_region_structure(spbub_deform);
    /*fp=fopen("region-info.dat","a");
      for (j=0;j<spbub_deform->bubbles.no_of_regions;j++){
      print_region_stats(&spbub_deform->bubbles.region[j],fp);
      }
      fclose(fp);*/
    if (verbose){
      printf("No. of regions =%d \n",spbub_deform->bubbles.no_of_regions);
      for (j=0;j<spbub_deform->bubbles.no_of_regions;j++){
    print_region_stats(&spbub_deform->bubbles.region[j],stdout);
      }
    }
    
    if (!silent){
      printf("Updating surface\n");
      fflush(stdout);
    }
    
    
    if (spbub_deform->params.save_surface != 0){
      save_2d_ED_bubbles_results(spbub_deform->params.outbase,
                 spbub_deform->bubbles.surface,
                 spbub_deform->bubbles.zero_level_set,
                 spbub_deform->params.alpha,
                 spbub_deform->params.beta,
                 spbub_deform->params.save_levelset,
                 spbub_deform->params.save_surface,i,
                 spbub_deform->params.band_size,
                 height,width);
    }
    if ((spbub_deform->params.save_surface != 0) && 
    (i % spbub_deform->params.save_surface == 0)){
      for (j=0;j<MAX_NO_REGIONS;j++){
    if (spbub_deform->bubbles.region[j].no_of_pixels > 0)
      print_region_stats(&spbub_deform->bubbles.region[j],stdout);
      }
      sprintf(reg_fname,"region-%04d.dat",i);
      /*//fp=fopen(reg_fname,"wb");
      //if (fp != NULL)
        //fwrite(spbub_deform->bubbles.region_label_array,sizeof(int),height*width,fp);
      //fclose(fp);
      //fflush(stdout);*/
    }

    if (!silent){
      printf("Done updating surface\n");
      fflush(stdout);
    }

    trace_eno_zero_xings_labelled(dsurface,&Tr,
                  spbub_deform->bubbles.region_label_array,
                  height,width);
    remove_xings_due_to_padding(&Tr, height, width, PAD);

    
   /* Do CEDT from the new ZEROX */

    /*Use the Tracer-results for finding the initial contour for CEDT*/
    spbub_deform->cedt.boundary_end=
      cedt_boundary_with_geno(&Tr, spbub_deform->cedt.boundary, height,width);
    free_data_QWaves_list(spbub_deform->waves.qwaves,height*width);
    initial_wave_prop_cedt_strucs(&spbub_deform->heap,&spbub_deform->cedt,
                  &spbub_deform->waves,height,width);
    if (!silent){
      printf("Going to do CEDT\n");
      fflush(stdout);
    }
    wave_propagation_via_cedt(&spbub_deform->heap, &spbub_deform->cedt, 
                  &spbub_deform->waves, height, width);

    if (!silent){
      printf("Done doing CEDT\n");
      fflush(stdout);
    }
    /*Re-initialize the surface after every 20 iteration */
    if (i!=0 && (i%15) == 0){
      CopyIm_DoubleToDouble(spbub_deform->bubbles.surface,dsurface,height,width);
      for (j=0;j<height*width;j++)
    if (dsurface[j] < 0.0){
      spbub_deform->bubbles.surface[j]=
        -1*sqrt(fabs(spbub_deform->cedt.surface[j]));
    }
    else{
      spbub_deform->bubbles.surface[j]=
        sqrt(fabs(spbub_deform->cedt.surface[j]));
    }
    }
    //eli edit free_data_tracer(&Tr);


  }

  free(dsurface);
  free(empty_image);
}

/**************************************************************************
  Description: Function that does the subpixel bubble evolution. Computes the
               appopriate fluxes and uses the forces (already computed) to 
           determine the speed values all over the image.
  Inputs :  spbub_deform - The SP_Bubble_Deform structure.
  Output :  spbub_deform 
 **************************************************************************/
void deform_SP_bubbles(SP_Bubble_Deform *spbub_deform)
{
  
  double reaction_flux_value, total_flux_value, diffusion_flux_value;
  double speed_value,snake_force,update_surf;
  double *update;
  double *dsurface;
  int height,width;
  int force_dir;
  int x,y,position;
  int verbose=0,write_files=1;
  FILE *fp;
  SP_Bubbles_Params *params;
  SP_Bubbles *bubbles;
  SP_Bubbles_Force *bub_force;

  height = spbub_deform->params.height;
  width = spbub_deform->params.width;
  bubbles = &spbub_deform->bubbles;
  params = &spbub_deform->params;
  bub_force = &spbub_deform->force;

  dsurface = (double *)calloc(height*width,sizeof(double));
  update = (double *)calloc(height*width,sizeof(double));
  /* CopyIm_DoubleToDouble(spbub_deform->bubbles.surface,dsurface,height,width); */


  /*Compute Reaction Flux values */
  if (spbub_deform->params.alpha != 0.0) {
    for (y=1;y<height-1;y++){
      for (x=1;x<width-1;x++){
    force_dir = bub_force->force_direction[y*width+x];
    reaction_flux_with_eno(bubbles->surface,spbub_deform->reaction,
                   params->band_size, params->alpha*force_dir,
                   spbub_deform->enosurface,x,y,height,width);
    /*reaction_flux(bubbles->surface,spbub_deform->reaction_array,
      params->band_size, params->alpha*force_dir,
      x,y,height,width); */
      }
    }
    adjust_boundaries_double(spbub_deform->reaction,height,width);
  }

  /*Compute Diffusion Flux values */
  if (spbub_deform->params.beta != 0.0) {
    for (y=1;y<height-1;y++){
      for (x=1;x<width-1;x++){
    force_dir = bub_force->force_direction[y*width+x];
    /*diffusion_flux_with_eno(bubbles->surface,spbub_deform->diffusion,
                params->band_size, spbub_deform->enosurface,
                x,y,height,width);*/
    /*diffusion_flux(bubbles->surface,spbub_deform->diffusion,
      params->band_size, x,y,height,width);*/
      }
    }
    adjust_boundaries_double(spbub_deform->diffusion,height,width);
  }

  /*Compute Snake Force values. */
  if (spbub_deform->params.gamma != 0.0) {
    for (y=1;y<height-1;y++){
      for (x=1;x<width-1;x++){
    /*Here we do S_x*phi_x+S_y*phi_y, where (S_x,S_y) are stop function derivatives
      and (phi_x,phi_y) are surface derivatives*/
    snake_force=find_grid_snake_force(bub_force->snake.Sx,bub_force->snake.Sy,
                      spbub_deform->enosurface,x,y,height,width);
    /*Here using second derivative in the normal direction*/
    /*bub_force->snake.snake_force[y*width+x]=snake_force; */
      }
    }
    adjust_boundaries_double(bub_force->snake.snake_force,height,width);
  }

  if (verbose){
    printf("Some Reaction values\n");
    for (y=13;y<=17;y++){
      for (x=11;x<=13;x++){
    printf("%f ",spbub_deform->reaction[y*width+x]);
      }
      printf("\n");
    }
    printf("Some Diffusion values\n");
    for (y=13;y<=17;y++){
      for (x=11;x<=13;x++){
    printf("%f ",spbub_deform->diffusion[y*width+x]);
      }
      printf("\n");
    }
  }
  /* Do Actual Surface Update (Osher-Sethian scheme is used) */
  for(y=2; y<(height-2); y++) {  
    for(x=2; x<(width-2); x++) {
      position = y*width+x;
      if (fabs(bubbles->surface[position]) < params->band_size) {
    if (params->gamma != 0.0) 
      snake_force = params->gamma*bub_force->snake.snake_force[position];
    else
      snake_force =0.0;
        speed_value = bub_force->stop[position]; 
        reaction_flux_value = spbub_deform->reaction[position];
        if (params->beta != 0.0) 
          diffusion_flux_value = spbub_deform->diffusion[position];
        else diffusion_flux_value = 0.0;
        /*total_flux_value = params->alpha*reaction_flux_value + 
           params->beta*diffusion_flux_value;
       update_surf =  params->delta_t*(speed_value*total_flux_value + snake_force); */
        total_flux_value = speed_value*params->alpha*reaction_flux_value + 
          params->beta*diffusion_flux_value;
    update_surf =  params->delta_t*(total_flux_value + snake_force);
    update[position] = update_surf;
    if (verbose){
      if (y>=110 && y<=115 && x>=10 && x<=15)
        printf("Loc=(%d,%d) Old=%f, Update=%f Rn=%f Df=%f\n",x,y,bubbles->surface[position],
           update_surf,reaction_flux_value,diffusion_flux_value);
    }
        bubbles->surface[position] += update_surf;
      } /* for y = 0... */
    }  /* for x = 0 ...*/
  }
  adjust_boundaries_pad_double(spbub_deform->bubbles.surface,height,width,2);

  CopyIm_DoubleToDouble(spbub_deform->bubbles.surface,dsurface,height,width);
  if (spbub_deform->enosurface)
    enoimage_free(spbub_deform->enosurface);
  spbub_deform->enosurface = enoimage_from_data_tbs(dsurface,height,width);
  


  if (write_files){
    /*//fp=fopen("diffusion.raw","w");
    //fwrite(spbub_deform->diffusion,sizeof(double),height*width,fp);
    //fclose(fp);
    //fp=fopen("reaction.raw","w");
    //fwrite(spbub_deform->reaction,sizeof(double),height*width,fp);
    //fclose(fp);
    //fp=fopen("update.raw","w");
    //fwrite(update,sizeof(double),height*width,fp);
    //fclose(fp);
    //fp=fopen("stop.raw","w");
    //fwrite(bub_force->stop,sizeof(double),height*width,fp);
    //fclose(fp);
    //fp=fopen("snake_force.raw","w");
    //fwrite(bub_force->snake.snake_force,sizeof(double),height*width,fp);
    //fclose(fp);*/
  }
  free(update);
  free(dsurface);
}/*End of function deform_SP_bubbles */

/**************************************************************************
  Description: Function that finds the zero level set using CEDT.
  Inputs :  cedt - The SP_CEDT structure.
            height,width - image dimensions
  Output :  zero_level_set - the sero level set array.
 **************************************************************************/

void get_zero_level_set_cedt(SPCedt *cedt, double *zero_level_set, int height, int width)
{
  int x,y,pos;

  for (y=0;y<height;y++){
    for (x=0;x<width;x++){
      pos=y*width+x;
      if (cedt->tag[pos] == 0 || cedt->tag[pos] == 1)
    zero_level_set[pos]=0;
      else
    zero_level_set[pos]=1;
    }
  }
}

#ifdef __cplusplus
}
#endif





