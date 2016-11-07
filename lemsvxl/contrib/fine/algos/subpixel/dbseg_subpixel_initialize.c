#ifdef __cplusplus
extern "C" {
#endif

#include "dbseg_subpixel_subpixel_bubbles.h"


/**************************************************************************
  Description: Function that allocates memory for SP_Bubble_Deform
  Inputs :  height,width - image dimensions
        pad - 0 -> no padding 1-> pad by one
        reg_count - no. of regions
  Output :  spbub_deform - filled SP_Bubble_Deform structure.

**************************************************************************/
void allocate_sp_bub_deform(SP_Bubble_Deform * spbub_deform,
                int height,int width,int reg_ct)
{
  int heap_size,i;

  /*spbub_deform->height = height;
    spbub_deform->width = width;*/

  /*printf("height=%d width=%d\n",height,width);*/

  allocate_sp_bubbles(&spbub_deform->bubbles,height,width,reg_ct);
  allocate_sp_bubbles_force(&spbub_deform->force,height,width);

  allocate_data_SP_Cedt(&spbub_deform->cedt, height, width);
  spbub_deform->cedt.boundary_end = 0;
  spbub_deform->cedt.boundary_start = 0;
  allocate_cedt_boundary(&spbub_deform->cedt.boundary,MAX_BOUNDARY_LENGTH);
  
  heap_size = height*width+5.0*MAX_BOUNDARY_LENGTH;
  alloc_mem_for_heap_SPCedt(&spbub_deform->heap,heap_size);
  initialize_heap_SPCedt(&spbub_deform->heap, heap_size);

  spbub_deform->cedt.ray=(Ray *)calloc(5.0*MAX_BOUNDARY_LENGTH,sizeof(Ray));
  spbub_deform->waves.qwaves=(QWaves *)calloc(height*width,sizeof(QWaves)); 
  spbub_deform->waves.qorder=(QuenchOrder *)calloc(height*width,sizeof(QuenchOrder));
  spbub_deform->waves.inter_boundary = 1;
  for (i=0;i<height*width;i++)
    spbub_deform->waves.qwaves[i].quench = NULL;
  
  spbub_deform->image = (double *)calloc(height*width,sizeof(double));
  spbub_deform->seed = (unsigned char *)calloc(height*width,sizeof(unsigned char));
  spbub_deform->reaction = (double *)calloc(height*width,sizeof(double));
  spbub_deform->diffusion = (double *)calloc(height*width,sizeof(double));
  spbub_deform->update = (double *)calloc(height*width,sizeof(double));

  spbub_deform->x_image_grad = (double *)calloc(height*width,sizeof(double));
  spbub_deform->y_image_grad = (double *)calloc(height*width,sizeof(double));
  spbub_deform->x_image_2grad = (double *)calloc(height*width,sizeof(double));
  spbub_deform->y_image_2grad = (double *)calloc(height*width,sizeof(double));
  spbub_deform->xy_image_grad = (double *)calloc(height*width,sizeof(double));
  spbub_deform->yx_image_grad = (double *)calloc(height*width,sizeof(double));
  
  /* spbub_deform->shocks.id = (ShId *)  calloc(10.0*height*width,sizeof(ShId)); */
  spbub_deform->shocks.id = (ShId *)  calloc(MAX_SHOCK_LENGTH,sizeof(ShId));
  spbub_deform->shocks.grid = (ShGrid *)  calloc(height*width,sizeof(ShGrid));
  spbub_deform->shocks.grouplist = (int *) calloc(height*width,sizeof(int));
  spbub_deform->back_shock_ptr = (int *)calloc(height*width,sizeof(int));

  printf("sizeof(Ray)=%d\n",sizeof(Ray));
  printf("sizeof(ShId)=%d\n",sizeof(ShId));
  printf("sizeof(ShGrid)=%d\n",sizeof(ShGrid));
  /*allocate_cedt_boundary(&spbub_deform->shock_boundary,MAX_BOUNDARY_LENGTH);*/
  spbub_deform->shock_boundary=(Boundary *)calloc(MAX_SHOCK_LENGTH,sizeof(Boundary));

}/* End of function allocate_sp_bub_deform */


/**************************************************************************
  Description: Function fills the SP_Bubble_Params data structure. 
  Inputs :  pad - 0 -> no padding 1-> pad by one
        iterations -> total no. of iterations
        alpha,beta -> bubble parameters
  Output :  params - filled SP_Bubble_Deform structure.

**************************************************************************/
void initialize_sp_bub_params(int pad, int iterations, double alpha, double beta, 
                  int height,int width,SP_Bubbles_Params *params)
{
  params->alpha = 0.3;/*alpha;*/
  params->beta = beta;
  params->gamma =0.0;
  params->total_no_iterations = iterations;
  params->start_from = 1;
  params->pad = pad;
  params->delta_x = 1.0;
  params->delta_y = 1.0;
  params->delta_t = 1.0;
  params->band_size = height*width;
  params->height=height+2*pad;
  params->width=width+2*pad;
}/*End of function initialize_sp_bub_params*/


/**************************************************************************
  Description: Function that allocates memory and fills the SP_Bubble_Deform
               data structure. 
  Inputs :  argc - no. of input arguments
            argv - the arguments
  Output :  spbub_deform - filled SP_Bubble_Deform structure.

**************************************************************************/
void initialize_sp_bub_deform_args(double* imageR, unsigned char* seedR, int hei, int wid, int numitr,
                   SP_Bubble_Deform * spbub_deform)
{
  int verbose=1;
  int reg_count=0,i;
  int height,width,pad;
  int x,y;
  int debug_flag=1;
  int seed_flag=0,surface_flag=0;
  /* int pix_ct[256]; */
  float *f_surface;
  double *surface;
  double x_grad,y_grad;
  double *dtmp;
  void *data;
  Point pt;
  Tracer Tr;
  FILE *fp,*fp1;

/*  if (argc < 3){
    usage();
    exit(1);
  }*/
  initialize_sp_bub_params_args(&spbub_deform->params,hei, wid, numitr);
  height = spbub_deform->params.height;
  width = spbub_deform->params.width;
  allocate_sp_bub_deform(spbub_deform,height,width,0);
  spbub_deform->seed=seedR;
  spbub_deform->image=imageR;
  read_input_bubble_arrays(spbub_deform->image,spbub_deform->seed,
               spbub_deform->bubbles.surface,
               spbub_deform->bubbles.region_label_array,
               5,&spbub_deform->params);
  printf("Reading Files Done\n");
  if (spbub_deform->params.start_from == 1){ seed_flag = 1; surface_flag = 0;} 
  else { seed_flag = 0; surface_flag = 1;}

  printf("Surf_flag=%d Seed_flag%d\n",surface_flag,seed_flag);

  dtmp = (double *)calloc(height*width,sizeof(double));
  if (!dtmp) error_msg("Memory allocation fault in initialize_bub_deform");
  
  /*If input is a seed*/
  if (seed_flag){
    if (spbub_deform->params.region_flag != 1)
      CopyIm_UCharToInt(spbub_deform->seed,spbub_deform->bubbles.region_label_array,
            height,width);
    /*Make the seed image binary */
    for (i=0;i<height*width;i++)
      if (spbub_deform->seed[i] > 0)
    spbub_deform->seed[i] = 1;
    /*CEDT on the seed image*/
    f_surface=(float *)calloc(height*width,sizeof(float));
    surface = (double *)calloc(height*width,sizeof(double));
    signed_distance_transform (spbub_deform->seed,f_surface,height,width);
    /* curvature_smoothing(f_surface, 0.3,3,0.0, height,width); */
    CopyIm_FloatToDouble(f_surface,surface,height,width);
    trace_eno_zero_xings_labelled(surface, &Tr,
                  spbub_deform->bubbles.region_label_array,
                  height,width);
    /*Recreate seed to fix  any changes  smoothing has made*/
    for (i=0;i<height*width;i++)
      if (surface[i] > 0)
    spbub_deform->seed[i] = 1;
      else
    spbub_deform->seed[i] = 0;
    
    
    printf("**** Going to do GENO *****\n");
    fflush(stdout);

    /*Use the Tracer-results for finding the initial contour for CEDT*/
    spbub_deform->cedt.boundary_end=
      cedt_boundary_with_geno(&Tr, spbub_deform->cedt.boundary, height,width);

    if (debug_flag){
      /*fp=fopen("pad-dist.raw","w");
      fwrite(surface,sizeof(double),(height)*(width),fp);
      fclose(fp);

      fp=fopen("seed-afterprocessing.raw","w");
      fwrite(spbub_deform->seed,sizeof(unsigned char),(height)*(width),fp);
      fclose(fp);*/

      printf("**** trying to print init_boundary *****\n");
      fflush(stdout);

      /*init_draw_ps_original("init-boundary.ps",5,height,width);
      draw_gridlines(0,0,height,width, YELLOW, data);
      draw_cedt_boundary_segment_zoom(spbub_deform->cedt.boundary,0,0,RED);
      close_draw_ps();*/
    }

    /*subpixel_cedt_from_image_labelled(spbub_deform->seed,&spbub_deform->cedt,
      &spbub_deform->heap, spbub_deform->params.curv_smooth,
      spbub_deform->bubbles.region_label_array,
      height,width);*/
    free_data_QWaves_list(spbub_deform->waves.qwaves,height*width);
    initial_wave_prop_cedt_strucs(&spbub_deform->heap,&spbub_deform->cedt,
                  &spbub_deform->waves,height,width);
    wave_propagation_via_cedt(&spbub_deform->heap, &spbub_deform->cedt, 
                  &spbub_deform->waves, height, width);
    CopyIm_DoubleToDouble(spbub_deform->cedt.surface,
              spbub_deform->bubbles.surface,height,width);
    /* change_surface_to_signed(&Tr,spbub_deform->bubbles.surface,height, width); */
    make_signed_distance_surface(spbub_deform->bubbles.surface,
                 spbub_deform->seed,height,width,1);
    if (debug_flag){
      printf("Done with cedt\n");
      fflush(stdout);
      /*fp=fopen("init-dist.raw","w");
      fwrite(spbub_deform->bubbles.surface,sizeof(double),(height)*(width),fp);
      fclose(fp);
      fp=fopen("cedt-dist.raw","w");
      fwrite(spbub_deform->cedt.surface,sizeof(double),(height)*(width),fp);
      fclose(fp);*/
      /* exit(1); */
    }
    /* CopyIm_DoubleToDouble(spbub_deform->cedt.surface,dtmp,height,width); */
    spbub_deform->enosurface = enoimage_from_data(spbub_deform->bubbles.surface,
                          height,width);
    reg_count = find_regions(spbub_deform->bubbles.region_label_array,height,width);
    //eli edit free_data_tracer(&Tr);
    free(f_surface);
    free(surface);
  }/*if (seed_flag)*/
  /*If input is a surface */
  else if(surface_flag){
    printf("Handling Surface NOW\n");
    for (i=0;i<height*width;i++)
      if (spbub_deform->bubbles.surface[i] > 0)
    spbub_deform->seed[i] = 1;
    /*CEDT from the surface image*/
    /* CopyIm_DoubleToDouble(spbub_deform->bubbles.surface,dtmp,height,width); */
    
    if (!spbub_deform->params.region_flag){
      trace_eno_zero_xings(spbub_deform->bubbles.surface, &Tr, height,width); 
      remove_xings_due_to_padding(&Tr, height, width, PAD);
      /*subpixel_cedt_pad(&spbub_deform->cedt,&Tr, spbub_deform->seed,height,width);*/
      spbub_deform->cedt.boundary_end=
    cedt_boundary_with_geno(&Tr, spbub_deform->cedt.boundary, height,width);
      free_data_QWaves_list(spbub_deform->waves.qwaves,height*width);
      initial_wave_prop_cedt_strucs(&spbub_deform->heap,&spbub_deform->cedt,
                    &spbub_deform->waves,height,width);
      wave_propagation_via_cedt(&spbub_deform->heap, &spbub_deform->cedt, 
                &spbub_deform->waves, height, width);
      
      /*Find ENO interpolation of surface*/
      /*CopyIm_DoubleToDouble(spbub_deform->cedt.surface,
    spbub_deform->bubbles.surface,height,width);
    change_surface_to_signed(&Tr,spbub_deform->bubbles.surface,height, width);*/
      spbub_deform->enosurface = enoimage_from_data(spbub_deform->bubbles.surface,
                            height,width);
      reg_count = fill_regions(spbub_deform->enosurface,
                   spbub_deform->bubbles.region_label_array,
                   spbub_deform->bubbles.surface);
      if (debug_flag){
    /*fp=fopen("cedt-surf.dat","w");
    fwrite(spbub_deform->cedt.surface,sizeof(double),height*width,fp);
    fclose(fp);
    fp=fopen("init-surf.dat","w");
    fwrite(spbub_deform->bubbles.surface,sizeof(double),height*width,fp);
    fclose(fp);*/
      }
      /* free_grid_label_list_all(&spbub_deform->cedt, height,width); */ 
      /* initialize_cedt(&spbub_deform->cedt, height,width); */
      //eli edit free_data_tracer(&Tr);
      /* exit(1); */
    }
    else{
      /*Find ENO interpolation of surface*/
      /* CopyIm_DoubleToDouble(spbub_deform->bubbles.surface,dtmp,height,width); */
      spbub_deform->enosurface = enoimage_from_data(spbub_deform->bubbles.surface,
                            height,width);
      /*for (i=0;i<=255;i++)
    pix_ct[i]=0;
    for (i=0;i<height*width;i++)
    pix_ct[spbub_deform->bubbles.region_label_array[i]]++;
    reg_count=0;
    for (i=1;i<=255;i++)
    if (pix_ct[i] > 0){
    if (verbose)
    printf("Region Label=%d No.=%d\n",i,pix_ct[i]);
    reg_count++;
    }*/
    }
    trace_eno_zero_xings_labelled(spbub_deform->bubbles.surface, &Tr,
                  spbub_deform->bubbles.region_label_array,
                  height,width);
    remove_xings_due_to_padding(&Tr, height, width, PAD);
    /* Do CEDT from the new ZEROX */
    /* free_grid_label_list_all(&spbub_deform->cedt, height,width);  */
    spbub_deform->cedt.boundary_end=
      cedt_boundary_with_geno(&Tr, spbub_deform->cedt.boundary, height,width);
    if (debug_flag){
      init_draw_ps_original("curvature.ps", 15, height,width);
      draw_gridlines(0,0,height,width, YELLOW, data);
      draw_cedt_boundary_segment_zoom(spbub_deform->cedt.boundary,0,0,RED);
      close_draw_ps();
    }
    free_data_QWaves_list(spbub_deform->waves.qwaves,height*width);
    initial_wave_prop_cedt_strucs(&spbub_deform->heap,&spbub_deform->cedt,
                  &spbub_deform->waves,height,width);
    wave_propagation_via_cedt(&spbub_deform->heap, &spbub_deform->cedt, 
                  &spbub_deform->waves, height, width);
    /*CopyIm_DoubleToDouble(spbub_deform->cedt.surface,
      spbub_deform->bubbles.surface,
      height,width);*/
    /* initialize_cedt(&spbub_deform->cedt, height,width); */
    /*subpixel_cedt_pad(&spbub_deform->cedt,&Tr,spbub_deform->seed,height,width);*/
    /*subpixel_cedt_from_image_labelled(spbub_deform->seed,&spbub_deform->cedt,
      &spbub_deform->heap, spbub_deform->params.curv_smooth,
      spbub_deform->bubbles.region_label_array,
      height,width);*/
    //eli edit free_data_tracer(&Tr);
  }/*else if (surface_flag)*/

  /* Do ENO interpolation for the image */
  CopyIm_DoubleToDouble(spbub_deform->image,dtmp,height,width);
  spbub_deform->enoimage = enoimage_from_data(dtmp,height,width);
  /*Fill in the bubbles structure */
  /*CopyIm_DoubleToDouble(spbub_deform->cedt.surface,spbub_deform->bubbles.surface,
    height,width);*/
  spbub_deform->bubbles.region=(Region_Stats *)calloc(MAX_NO_REGIONS+2,
                              sizeof(Region_Stats));
  spbub_deform->bubbles.no_of_regions = reg_count;
  get_level_set_surface(spbub_deform->bubbles.surface, 
            spbub_deform->bubbles.zero_level_set,
            0,0,1,spbub_deform->params.band_size,height,width); 
  /* Fill in the Region_Stats structure in bubbles*/
  /*for (i=0;i<reg_count;i++){
    find_mean_stdev_region(spbub_deform->image,spbub_deform->bubbles.region_label_array,
    i+1, height,width,&spbub_deform->bubbles.region[i]);*/
  if (spbub_deform->params.no_of_classes == 1){
    update_region_stats(spbub_deform->bubbles.region_label_array, spbub_deform->image, 
            spbub_deform->bubbles.region,spbub_deform->params.no_of_classes,
            height,width);
  }
  else if (spbub_deform->params.no_of_classes == 2){
    update_region_stats(spbub_deform->bubbles.region_label_array, spbub_deform->image, 
            spbub_deform->bubbles.region,spbub_deform->params.no_of_classes-1,
            height,width);

    for (i=1;i<=MAX_NO_REGIONS-1;i++){
      if (spbub_deform->bubbles.region[i].no_of_pixels > 0){
    spbub_deform->bubbles.region[i].mean_vec[0]=spbub_deform->bubbles.region[i].mean-
      spbub_deform->bubbles.region[i].stdev;
    spbub_deform->bubbles.region[i].mean_vec[1]=spbub_deform->bubbles.region[i].mean+
      spbub_deform->bubbles.region[i].stdev;
    spbub_deform->bubbles.region[i].std_vec[0]=spbub_deform->bubbles.region[i].stdev/2;
    spbub_deform->bubbles.region[i].std_vec[1]=spbub_deform->bubbles.region[i].stdev/2;
    spbub_deform->bubbles.region[i].alpha_vec[0]=0.5;
    spbub_deform->bubbles.region[i].alpha_vec[1]=0.5;
      }
    }
    update_region_stats(spbub_deform->bubbles.region_label_array, spbub_deform->image, 
            spbub_deform->bubbles.region,spbub_deform->params.no_of_classes,
            height,width);
    
  }
  else{
    printf("invalid no. of classes\n");
    exit(1);
  }
    save_2d_ED_bubbles_results(spbub_deform->params.outbase,spbub_deform->bubbles.surface,
                   spbub_deform->bubbles.zero_level_set,
                   spbub_deform->params.alpha,
                   spbub_deform->params.beta,
                   spbub_deform->params.save_levelset,
                   spbub_deform->params.save_surface,0,
                   spbub_deform->params.band_size,
                   height,width);
  /*Fill the image gradients*/
  for (y=1;y<height-1;y++){
    for (x=1;x<width-1;x++){
      pt.x = (double)x;
      pt.y = (double)y;
      /*Find Ixx & Iyy*/
      find_grid_eno_2nd_der(pt,spbub_deform->enoimage,&x_grad,&y_grad);
      spbub_deform->x_image_2grad[y*width+x] = x_grad;
      spbub_deform->y_image_2grad[y*width+x] = y_grad;
      /*Find Ix & Iy*/
      find_grid_eno_gradient(pt,spbub_deform->enoimage,&x_grad,&y_grad);
      spbub_deform->x_image_grad[y*width+x] = x_grad;
      spbub_deform->y_image_grad[y*width+x] = y_grad;
    }
  }
  adjust_boundaries_double(spbub_deform->x_image_grad,height,width);
  adjust_boundaries_double(spbub_deform->y_image_grad,height,width);
  adjust_boundaries_double(spbub_deform->x_image_2grad,height,width);
  adjust_boundaries_double(spbub_deform->y_image_2grad,height,width);

  /*Find Ixy & Iyx*/
  for (y=1;y<height-1;y++){
    for (x=1;x<width-1;x++){
      spbub_deform->xy_image_grad[y*width+x] = 
    (spbub_deform->x_image_grad[(y+1)*width+x]-
     spbub_deform->x_image_grad[(y-1)*width+x])/2;
      spbub_deform->yx_image_grad[y*width+x] = 
    (spbub_deform->y_image_grad[y*width+x+1]-
     spbub_deform->y_image_grad[y*width+x-1])/2;
    }
  }
  adjust_boundaries_double(spbub_deform->xy_image_grad,height,width);
  adjust_boundaries_double(spbub_deform->xy_image_grad,height,width);
    

  /*Line Force (currently using logical linear)*/
  if (spbub_deform->params.line_flag == 1){
    fp1=fopen(spbub_deform->params.line_file,"r");
    if (fp1 == NULL){
      printf("Error reading line force file\n");
      exit(1);
    }
    else{
      fread(spbub_deform->force.line_force,sizeof(double),height*width,fp1);
      fclose(fp1);
    }
    /*fp=fopen("line-force.dat","w");
    fwrite(spbub_deform->force.line_force,sizeof(double),height*width,fp);
    fclose(fp);*/
  }
  
  if (debug_flag){
    /*init_draw_ps_original("curvature.ps", 5, height,width);
    draw_gridlines(0,0,height,width, YELLOW, data);
    draw_cedt_boundary_segment_zoom(spbub_deform->cedt.boundary,0,0,RED);
    close_draw_ps();

    fp=fopen("seed.dat","w");
    fwrite(spbub_deform->seed,sizeof(unsigned char),height*width,fp);
    fclose(fp);
    
    fp=fopen("cedt-surf.dat","w");
    fwrite(spbub_deform->cedt.surface,sizeof(double),height*width,fp);
    fclose(fp);

    fp=fopen("init-surf.dat","w");
    fwrite(spbub_deform->bubbles.surface,sizeof(double),height*width,fp);
    fclose(fp);

    fp=fopen("init-region.dat","w");
    fwrite(spbub_deform->bubbles.region_label_array,sizeof(int),height*width,fp);
    fclose(fp);

    fp=fopen("image.dat","w");
    fwrite(spbub_deform->image,sizeof(double),height*width,fp);
    fclose(fp);
     exit(1); 
    fp = fopen("xgrad.raw","w");
    fwrite(spbub_deform->x_image_grad,sizeof(double),height*width,fp);
    fclose(fp);
    fp = fopen("ygrad.raw","w");
    fwrite(spbub_deform->y_image_grad,sizeof(double),height*width,fp);
    fclose(fp);
    fp = fopen("xygrad.raw","w");
    fwrite(spbub_deform->xy_image_grad,sizeof(double),height*width,fp);
    fclose(fp);
    fp = fopen("yxgrad.raw","w");
    fwrite(spbub_deform->yx_image_grad,sizeof(double),height*width,fp);
    fclose(fp);
    fp = fopen("x2grad.raw","w");
    fwrite(spbub_deform->x_image_2grad,sizeof(double),height*width,fp);
    fclose(fp);
    fp = fopen("y2grad.raw","w");
    fwrite(spbub_deform->y_image_2grad,sizeof(double),height*width,fp);
    fclose(fp);*/
    
  }
  free(dtmp);

}/* End of function initialize_sp_bub_deform_args */





/**************************************************************************
  Description: Function that frees memory allocated to the SP_Bubble_Deform
               data structure. 
  Inputs : spbub_deform Pointer to  SP_Bubble_Deform.

**************************************************************************/
void free_sp_bub_deform(SP_Bubble_Deform * spbub_deform)
{

  int height,width;
  height=spbub_deform->params.height;
  width=spbub_deform->params.width;

  free_sp_bubbles(&spbub_deform->bubbles);
  free_sp_bubbles_force(&spbub_deform->force);
  free(spbub_deform->image);
  free(spbub_deform->seed);
  free(spbub_deform->reaction);
  free(spbub_deform->diffusion);
  free(spbub_deform->update);
  free(spbub_deform->x_image_grad);
  free(spbub_deform->y_image_grad);
  free(spbub_deform->x_image_2grad);
  free(spbub_deform->y_image_2grad);
  free(spbub_deform->xy_image_grad);
  free(spbub_deform->yx_image_grad);

  enoimage_free(spbub_deform->enoimage); 
  enoimage_free(spbub_deform->enosurface);
  free_data_SP_Cedt(&spbub_deform->cedt);
  free_mem_heap_SP_Cedt(&spbub_deform->heap);
  free(spbub_deform->cedt.ray);
  free_cedt_boundary(spbub_deform->cedt.boundary,MAX_BOUNDARY_LENGTH);
  free_data_QWaves_list(spbub_deform->waves.qwaves,height*width);
  free(spbub_deform->waves.qwaves);
  free(spbub_deform->waves.qorder);

  free(spbub_deform->back_shock_ptr);
  /*free_cedt_boundary(spbub_deform->linear_boundary,MAX_BOUNDARY_LENGTH);*/
  /*free_cedt_boundary(spbub_deform->shock_boundary,MAX_BOUNDARY_LENGTH);*/
  free(spbub_deform->shock_boundary);

  free_shock_grids_ptrs(&spbub_deform->shocks,height,width);
  free(spbub_deform->shocks.id);
  free(spbub_deform->shocks.grid);
  free(spbub_deform->shocks.grouplist);
  


}/* End of function free_sp_bub_deform */


#ifdef __cplusplus
}
#endif





