#ifdef __cplusplus
extern "C" {
#endif

#include "dbseg_subpixel_subpixel_bubbles.h"

void run_scdm_raph(double alpha, double beta, double gamma,
           int no_iter, int start_from, int pad,
           int height, int width, int save_levelset,
           int save_surface, double gauss_smooth, 
           int curv_smooth, int region_flag, char *outbase,
           int *in_region, unsigned char * in_seed,
           double *in_surface, double *in_image)
{
  SP_Bubble_Deform spbub_deform;
  FILE *fp;
  int i,j;
  
  spbub_deform.params.alpha = alpha;
  spbub_deform.params.beta = beta;
  spbub_deform.params.gamma = gamma;
  spbub_deform.params.total_no_iterations = no_iter;
  spbub_deform.params.start_from = start_from;
  spbub_deform.params.pad = pad;
  spbub_deform.params.delta_x = 1.0;
  spbub_deform.params.delta_y = 1.0;
  spbub_deform.params.delta_t = 1.0;
  spbub_deform.params.height=height;
  spbub_deform.params.width=width;
  spbub_deform.params.band_size = height*width;
  spbub_deform.params.save_levelset = save_levelset;
  spbub_deform.params.save_surface = save_surface;
  spbub_deform.params.gauss_smooth = gauss_smooth;
  spbub_deform.params.curv_smooth = curv_smooth;
  spbub_deform.params.region_flag = region_flag;
  strcpy(spbub_deform.params.outbase,outbase);

  print_parameters(&spbub_deform.params);

  initialize_sp_bub_deform_raph(in_region,in_seed,in_surface, 
                in_image,&spbub_deform);
  printf("!!!!!!!!!!!!!!\n!!!!!!!!!!!!!!\n!!!!!!!!!!!!!!\nIn C function SCDM\n");
  print_parameters(&spbub_deform.params);

  for (j=0;j<MAX_NO_REGIONS;j++){
    if (spbub_deform.bubbles.region[j].no_of_pixels > 0)
      print_region_stats(&spbub_deform.bubbles.region[j],stdout);
  }

  subpixel_bubble_evolution(&spbub_deform); 
  
  fp=fopen("dist.raw","w");
  fwrite(spbub_deform.bubbles.surface,sizeof(double),
     height*width,fp);
  fclose(fp);
  CopyIm_DoubleToDouble(spbub_deform.bubbles.surface,in_surface,height,width);
  CopyIntToInt(spbub_deform.bubbles.region_label_array,in_region,1,height,width);
  free_sp_bub_deform(&spbub_deform);

}  

/**************************************************************************
  Description: Function that allocates memory and fills the SP_Bubble_Deform
               data structure. 
  Inputs :  argc - no. of input arguments
            argv - the arguments
  Output :  spbub_deform - filled SP_Bubble_Deform structure.

**************************************************************************/
void initialize_sp_bub_deform_raph(int *in_region, unsigned char * in_seed,
                   double *in_surface, double *in_image,
                   SP_Bubble_Deform * spbub_deform)
{
  unsigned char *seed;
  int verbose=1;
  int reg_count=0,i;
  int height,width,pad;
  int x,y;
  int debug_flag=0;
  int seed_flag=0,surface_flag=0;
  /* int pix_ct[256]; */
  float *f_surface;
  double *image,*surface;
  double x_grad,y_grad;
  double *dtmp;
  void *data;
  Point pt;
  Tracer Tr;
  FILE *fp;

  /*initialize_sp_bub_params_args(&spbub_deform->params,argc,argv);*/

  height = spbub_deform->params.height;
  width = spbub_deform->params.width;
  allocate_sp_bub_deform(spbub_deform,height,width,0);
  read_input_bubble_arrays_raphael(spbub_deform->image,spbub_deform->seed,
                   spbub_deform->bubbles.surface,
                   spbub_deform->bubbles.region_label_array,
                   in_image, in_seed,in_surface,in_region, 
                   &spbub_deform->params);

  if (spbub_deform->params.start_from == 1){ seed_flag = 1; surface_flag = 0;} 
  else { seed_flag = 0; surface_flag = 1;}

  if (debug_flag){
    printf("Surf_flag=%d Seed_flag%d\n",surface_flag,seed_flag);
  }
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
    
    
    /*Use the Tracer-results for finding the initial contour for CEDT*/
    spbub_deform->cedt.boundary_end=
      cedt_boundary_with_geno(&Tr, spbub_deform->cedt.boundary, height,width);

    if (debug_flag){
      fp=fopen("pad-dist.raw","w");
      fwrite(surface,sizeof(double),(height)*(width),fp);
      fclose(fp);

      init_draw_ps_original("init-boundary.ps",5,height,width);
      draw_gridlines(0,0,height,width, YELLOW, data);
      draw_cedt_boundary_segment_zoom(spbub_deform->cedt.boundary,0,0,RED);
      close_draw_ps();
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
      fp=fopen("pad-dist.raw","w");
      fwrite(surface,sizeof(double),(height)*(width),fp);
      fclose(fp);
      printf("Done with cedt\n");
      fflush(stdout);
      fp=fopen("init-dist.raw","w");
      fwrite(spbub_deform->bubbles.surface,sizeof(double),(height)*(width),fp);
      fclose(fp);
      fp=fopen("cedt-dist.raw","w");
      fwrite(spbub_deform->cedt.surface,sizeof(double),(height)*(width),fp);
      fclose(fp);
      /* exit(1); */
    }
    /* CopyIm_DoubleToDouble(spbub_deform->cedt.surface,dtmp,height,width); */
    spbub_deform->enosurface = enoimage_from_data(spbub_deform->bubbles.surface,
                          height,width);
    reg_count = find_regions(spbub_deform->bubbles.region_label_array,height,width);
    //eli edit free_data_tracer(&Tr);
    free(f_surface);
    free(surface);
  }
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
    fp=fopen("pad-dist.raw","w");
    fwrite(surface,sizeof(double),(height)*(width),fp);
    fclose(fp);
    fp=fopen("cedt-surf.dat","w");
    fwrite(spbub_deform->cedt.surface,sizeof(double),height*width,fp);
    fclose(fp);
      
    fp=fopen("init-surf.dat","w");
    fwrite(spbub_deform->bubbles.surface,sizeof(double),height*width,fp);
    fclose(fp);
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
  update_region_stats(spbub_deform->bubbles.region_label_array, spbub_deform->image, 
              spbub_deform->bubbles.region,spbub_deform->params.no_of_classes,height,width);
  
  if (1)
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
    

  if (debug_flag){
    init_draw_ps_original("curvature.ps", 15, height,width);
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
    /* exit(1); */
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
    fclose(fp);
    
  }
  free(dtmp);

}/* End of function initialize_sp_bub_deform_args */



/***********************************************************************
  Description - Read the input arrays for subpixel bubbles. If starting
                from 1, image and seed are inputs else image and surface
        are inputs.
  Input : image - Image array
  seed  - Seed array
      surface - Surface array
  Output :params - SP_Bubbles_Params data structure
***********************************************************************/
void read_input_bubble_arrays_raphael(double *image, unsigned char *seed, 
                      double *surface, int *region, 
                      double *in_image, unsigned char *in_seed, 
                      double *in_surface, int *in_region, 
                      SP_Bubbles_Params *params)
{
  int i,h,w,ph,pw,readct;
  /*float *f_tmp_image,*f_tmp_surface,*f_gauss_image;*/
  double *gauss_image;
  FILE *fp;

  h=params->height-2*params->pad;
  w=params->width-2*params->pad;
  ph=params->height;
  pw=params->width;
  /*f_tmp_image = (float *)calloc(h*w,sizeof(float));
    f_tmp_surface = (float *)calloc(h*w,sizeof(float));*/

  if (params->gauss_smooth > 0.0){
    /*Gaussian smooth the image */
    /* CopyIm_DoubleToFloat(in_image,f_tmp_image,h,w); */
    gauss_image = (double *)calloc(h*w,sizeof(double));
    gauss_conv(in_image,gauss_image,params->gauss_smooth,params->gauss_smooth,
           1+(int)3*params->gauss_smooth,1,h,w);
    free(gauss_image);
  }
  if (params->pad == 0)
    CopyIm_DoubleToDouble(in_image,image,h,w);
  else if (params->pad==1)
    PadIm_DoubleToDoubleP(in_image,image,ph,pw,params->pad);
  else
    error_msg("Currently handles pad of 0/1 only");
  
  /*Read Seed/Surface array */
  
  /*Seed*/
  if (params->start_from == 1){
    if (params->pad == 0)
      CopyIm_UCharToUChar(in_seed,seed,h,w);
    else if (params->pad==1)
      PadIm_UCharToUCharP(in_seed,seed,ph,pw,params->pad);
    else
      error_msg("Currently handles pad of 0/1 only");
  }
  /*Surface*/
  else{
    if (params->curv_smooth > 0.0){
      /* CopyIm_DoubleToFloat(in_surface,f_tmp_surface,h,w); */
      curvature_smoothing(in_surface,0.3,params->curv_smooth,0.0,h,w);
      /* CopyIm_FloatToDouble(f_tmp_surface,in_surface,h,w); */
    }
    for (i=0;i<h*w;i++)
      if (in_surface[i] > 0)
    in_seed[i]=255;
    if (params->pad == 0){
      CopyIm_UCharToUChar(in_seed,seed,h,w);
      CopyIm_DoubleToDouble(in_surface,surface,h,w);
    }
    else if (params->pad==1){
      PadIm_UCharToUCharP(in_seed,seed,ph,pw,params->pad);
      PadIm_DoubleToDoubleP(in_surface,surface,ph,pw,params->pad);
    }
    else
      error_msg("Currently handles pad of 0/1 only");
  }

  /*Region */
  if (params->region_flag == 1){
    if (params->pad == 0)
      CopyIntToInt(in_region,region,1,h,w);
    else if (params->pad==1)
      PadIm_IntToIntP(in_region,region,ph,pw,params->pad);
    else
      error_msg("Currently handles pad of 0/1 only");
  }

  fp=fopen("inp-surf.raw","w");
  fwrite(surface,sizeof(double),h*w,fp);
  fclose(fp);
  
  fp=fopen("inp-seed.raw","w");
  fwrite(seed,sizeof(unsigned char),h*w,fp);
  fclose(fp);
  
  fp=fopen("inp-regn.raw","w");
  fwrite(region,sizeof(int),h*w,fp);
  fclose(fp);
  
  

  /*free(f_tmp_surface);
    free(f_tmp_image);*/
}


#ifdef __cplusplus
}
#endif





