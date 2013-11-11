#ifdef __cplusplus
extern "C" {
#endif

#include "dbseg_subpixel_subpixel_bubbles.h"

/**************************************************************************
  Description: Function  fills the SP_Bubble_Params data structure using the
               the input arguments.
  Inputs :  argc - no. of arguments
            argv - list of arguments
  Output :  params - filled SP_Bubble_Deform structure.

**************************************************************************/
void initialize_sp_bub_params_args(SP_Bubbles_Params *params, int height, int width, int numitr)   //void initialize_sp_bub_params_args(SP_Bubbles_Params *params, int argc, char *argv[])
{
  /*Set default values for all arguments*/
  params->alpha = -0.2;
  params->beta = 0.025;
  params->gamma =0.0;
  params->total_no_iterations = 1;
  params->start_from = 1;
  params->pad = 0;
  params->delta_x = 1.0;
  params->delta_y = 1.0;
  params->delta_t = 1.0;
  params->height=150;
  params->width=150;
  params->band_size = 150*150;
  params->save_levelset = 0;
  params->save_surface = 1;
  params->gauss_smooth = 0.0;
  params->curv_smooth = 0;
  params->region_flag = 0;
  params->line_flag=0;
  params->no_of_classes=1;
  //strcpy(params->outbase,argv[1]);
  
  

  params->height = height;
  params->width = width;
  params->total_no_iterations = numitr;
  /*
  for (--argc, ++argv; argc; --argc, ++argv ) {
    
    if ( **argv == '-' ) {  
      ++*argv;
      --argc;
      
      if ( keymatch( *argv, "alpha", 5))
        params->alpha = atof( *++argv);
      else if (keymatch( *argv, "beta", 4))
        params->beta = atof( *++argv);
      else if (keymatch( *argv, "gamma", 5))
        params->gamma = atof( *++argv);
      else if (keymatch( *argv, "height", 6))
        params->height = atoi( *++argv);
      else if (keymatch( *argv, "width", 5))
        params->width = atoi( *++argv);
      else if (keymatch( *argv, "pad", 3))
        params->pad = atoi( *++argv);
      else if (keymatch( *argv, "steps", 5))
        params->total_no_iterations = atoi( *++argv);
      else if (keymatch( *argv, "start_from", 10))
        params->start_from = atoi( *++argv);
      else if (keymatch( *argv, "save_levelset", 13))
        params->save_levelset = atoi( *++argv);
      else if (keymatch( *argv, "save_surface", 12))
        params->save_surface = atoi( *++argv);
      else if (keymatch( *argv, "band_size", 9))
        params->band_size = atoi( *++argv);
      else if (keymatch( *argv, "region", 6)){
        strcpy(params->region_file,*++argv);
    params->region_flag = 1;
      }
      else if (keymatch( *argv, "line", 4)){
        strcpy(params->line_file,*++argv);
    params->line_flag = 1;
      }
      else if (keymatch( *argv, "outbase", 7)){
    ++argv;
        strcpy(params->outbase,*argv);
      }
      else if (keymatch( *argv, "gauss_smooth", 12))
        params->gauss_smooth = atof( *++argv);
      else if (keymatch( *argv, "curv_smooth", 11))
        params->curv_smooth = atoi( *++argv);
      else if (keymatch( *argv, "no_of_classes", 13))
        params->no_of_classes = atoi( *++argv);
      else{
    printf("%s is an Invalid argument\n",*argv);
    usage();
    exit(1);
      }
    }
  }*/
  printf("alpha=%f beta=%f\n",params->alpha,params->beta);
  params->height += 2*params->pad;
  params->width += 2*params->pad;
  printf("height=%d width=%d\n",params->height,params->width);
  
  
}/*End of function initialize_sp_bub_params_args*/

/***********************************************************************
  Description - Prints the usage for the function
                                         
***********************************************************************/
void usage()
{
  printf("Usage for subpixel-bubbles:\n");
  printf("subpixel-bubbles \n");
  printf("    image-file-name - Mandatory input\n");
  printf("    seed/surface file name - Mandatory input\n");
  printf("optional inputs\n");
  printf("    -alpha\n");
  printf("    -beta\n");
  printf("    -gamma\n");
  printf("    -height\n");
  printf("    -width\n");
  printf("    -pad\n");
  printf("    -steps\n");
  printf("    -start_from\n");
  printf("    -save_levelset\n");
  printf("    -save_surface\n");
  printf("    -band_size\n");
  printf("    -outbase\n");
  printf("    -gauss_smooth\n");
  printf("    -curv_smooth\n");
  printf("    -region\n");
  printf("    -no_of_classes\n");
}

/***********************************************************************
  Description - Prints the current parameter values
  Input : params - SP_Bubbles_Params data structure
***********************************************************************/
void print_parameters(SP_Bubbles_Params *params)
{
  printf("    -alpha         %5.2f\n",params->alpha);
  printf("    -beta          %5.2f\n",params->beta);
  printf("    -gamma         %5.2f\n",params->gamma);
  printf("    -height        %d\n",params->height);
  printf("    -width         %d\n",params->width);
  printf("    -pad         %d\n",params->pad);
  printf("    -steps         %d\n",params->total_no_iterations);
  printf("    -start_from    %d\n", params->start_from);
  printf("    -save_levelset %d\n",params->save_levelset);
  printf("    -save_surface  %d\n",params->save_surface);
  printf("    -outbase       %s\n",params->outbase);
  printf("    -gauss_smooth  %5.2f\n",params->gauss_smooth);
  printf("    -curv_smooth  %5.2f\n",params->curv_smooth);
  printf("    -region_file %s\n",params->region_file);
}

/***********************************************************************
  Description - Read the input arrays for subpixel bubbles. If starting
                from 1, image and seed are inputs else image and surface
        are inputs.
  Input : image - Image array
  seed  - Seed array
      surface - Surface array
  Output :params - SP_Bubbles_Params data structure
***********************************************************************/
void read_input_bubble_arrays(double *image, unsigned char *seed, double *surface,
                  int *region, char *argv[],SP_Bubbles_Params *params)
{
  int i,h,w,ph,pw,readct;
  int *tmp_region,debug_flag;
  unsigned char *tmp_seed;
  double *tmp_image,*tmp_surface;
  float *f_tmp_image,*f_tmp_surface;
  double *gauss_image;
  FILE *fp;

  h=params->height-2*params->pad;
  w=params->width-2*params->pad;
  ph=params->height;
  pw=params->width;
  tmp_region = (int *)calloc(h*w,sizeof(int));
  f_tmp_image = (float *)calloc(h*w,sizeof(float));
  f_tmp_surface = (float *)calloc(h*w,sizeof(float));
  tmp_image = (double *)calloc(h*w,sizeof(double));
  tmp_surface = (double *)calloc(h*w,sizeof(double));
  tmp_seed = (unsigned char *)calloc(h*w,sizeof(unsigned char));

  /*Reading Input Image (grayscale)*/
  /*
  fp=fopen(argv[1],"r"); 
  if (fp == NULL) error_msg("File opening error: Image  file couldn't be opened");
  readct=fread(tmp_image,sizeof(double),h*w,fp);
  if (readct != h*w) error_msg("File read error: Error reading image file");
  fclose(fp);*/
  tmp_image=image;
  tmp_seed = seed;

  if (params->gauss_smooth > 0.0){
    /*Gaussian smooth the image */
    gauss_image = (double *)calloc(h*w,sizeof(double));
    gauss_conv(tmp_image,gauss_image,params->gauss_smooth,params->gauss_smooth,
           1+(int)3*params->gauss_smooth,1,h,w);
    CopyIm_DoubleToDouble(gauss_image,tmp_image,h,w);
    free(gauss_image);
  }
  if (params->pad == 0)
    CopyIm_DoubleToDouble(tmp_image,image,h,w);
  else if (params->pad==1)
    PadIm_DoubleToDoubleP(tmp_image,image,ph,pw,params->pad);
  else
    error_msg("Currently handles pad of 0/1 only");
  
  /*Read Seed/Surface array */
  
  /*Seed*/
  /*
  if (params->start_from == 1){
    fp=fopen(argv[2],"rb"); 
    if (fp == NULL) error_msg("File opening error: Seed  file couldn't be opened");
    readct=fread(tmp_seed,sizeof(unsigned char),h*w,fp);
    if (readct != h*w) error_msg("File read error: Error reading seed file");
    fclose(fp);
    if (params->pad == 0)
      CopyIm_UCharToUChar(tmp_seed,seed,h,w);
    else if (params->pad==1)
      PadIm_UCharToUCharP(tmp_seed,seed,ph,pw,params->pad);
    else
      error_msg("Currently handles pad of 0/1 only");
  }*/
  
  
  
  
  /*Surface*/
  /*
  else{
    fp=fopen(argv[2],"rb"); 
    printf("%s\n",argv[2]);
    if (fp == NULL) error_msg("File opening error: Seed  file couldn't be opened");
    readct=fread(tmp_surface,sizeof(double),h*w,fp);
    if (readct != h*w) { printf("%d\n",readct);error_msg("File read error: Error reading surface file");}
    fclose(fp);
    if (params->curv_smooth > 0.0){
      curvature_smoothing(tmp_surface,0.3,params->curv_smooth,0.0,h,w);
    }
    for (i=0;i<h*w;i++)
      if (tmp_surface[i] > 0)
    tmp_seed[i]=255;
    if (params->pad == 0){
      CopyIm_UCharToUChar(tmp_seed,seed,h,w);
      CopyIm_DoubleToDouble(tmp_surface,surface,h,w);
    }
    else if (params->pad==1){
      PadIm_UCharToUCharP(tmp_seed,seed,ph,pw,params->pad);
      PadIm_DoubleToDoubleP(tmp_surface,surface,ph,pw,params->pad);
    }
    else
      error_msg("Currently handles pad of 0/1 only");
  }*/

  /*Region */

  /*
  if (params->region_flag == 1){
    fp=fopen(params->region_file,"rb"); 
    if (fp == NULL) error_msg("File opening error: Region  file couldn't be opened");
    readct=fread(tmp_region,sizeof(int),h*w,fp);
    if (readct != h*w) error_msg("File read error: Error reading region file");
    fclose(fp);
    if (params->pad == 0)
      CopyIntToInt(tmp_region,region,1,h,w);
    else if (params->pad==1)
      PadIm_IntToIntP(tmp_region,region,ph,pw,params->pad);
    else
      error_msg("Currently handles pad of 0/1 only");
  }
  */

/*
  if (debug_flag){
    fp=fopen("inp-surf.raw","w");
    fwrite(surface,sizeof(double),h*w,fp);
    fclose(fp);
    
    fp=fopen("inp-seed.raw","w");
    fwrite(seed,sizeof(unsigned char),h*w,fp);
    fclose(fp);
    
    fp=fopen("inp-regn.raw","w");
    fwrite(region,sizeof(int),h*w,fp);
    fclose(fp);
  }
*/
  

  free(tmp_seed);
  free(tmp_region);
  free(tmp_surface);
  free(tmp_image);
  free(f_tmp_surface);
  free(f_tmp_image);
}

#ifdef __cplusplus
}
#endif





