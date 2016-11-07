#ifdef __cplusplus
extern "C" {
#endif

#include "dbseg_subpixel_subpixel_bubbles.h"

void save_2d_ED_bubbles_results(char *outimage, double *surface_array, double *level_set_array, 
                             double alpha, double beta, int how_often_save,int save_surface,
                             int iteration, double band_size, int height, int width)

{
  int err;
  float *f_level_set_array;
  PgmImage pgmim;
  FILE *fopen(),*f1;
  char out[128]="";
  
  strcpy(out,outimage);
  /* f1 = (FILE *)malloc(sizeof(FILE)); */
  
  InitPgm(&pgmim);
  pgmim.w=width-2;
  pgmim.h=height-2;
  pgmim.im = (unsigned char *) calloc(pgmim.w*pgmim.h,sizeof(unsigned char)); 
  
  /* save the bubbles */
  if (how_often_save != 0 && ((iteration%how_often_save == 0) || (iteration == 1))) { 
   /* get_level_set (double *surface_array, double *level_set_array, double level, 
     int boundary_value,  int interpolate, int band_size,int height, int width); */
    f_level_set_array=(float *)calloc(height*width,sizeof(float));
    get_level_set_surface(surface_array, level_set_array,0,0,1,
              band_size,height,width); 
    CopyIm_DoubleToFloat(level_set_array,f_level_set_array,height,width);
    CopyZLevelToUByte(f_level_set_array,pgmim.im,(height-2),(width-2));
    
    /* printf("the color type is:%d\n",thePInfo.colType); */                     
    sprintf(outimage,"%s-%1.2f-%1.2f-%04d.pgm",out,alpha,beta,
            iteration);
    /*        printf("saving the raster image to: %s\n",outimage);  */
    write_pgmimage(outimage,&pgmim);
    free(f_level_set_array);
  }
 
  if ((save_surface != 0) && (iteration%save_surface ==0 || iteration==1)) {
    /*sprintf(outimage,"%s-%1.2f-%1.2f-%04d.raw",out,alpha,beta,
                iteration);
    printf("saving the raw image to: %s\n",outimage);  
    f1 = fopen(outimage,"wb");
    if (f1 == 0) { printf("no such directory\n"); exit(5); }
    fwrite(surface_array, sizeof(double), (height*width), f1);
    fclose(f1);*/
  }
 
  free(pgmim.im);
  strcpy(outimage,out);

}

#ifdef __cplusplus
}
#endif





