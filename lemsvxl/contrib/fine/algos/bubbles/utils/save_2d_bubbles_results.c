#include <stdio.h>
#include <math.h>
#include <iiointerface.h> 

#include <utils.h>

void save_2d_ED_bubbles_results(char *outimage, float *surface_array, float *level_set_array, 
                             float alpha, float beta, int how_often_save,int save_surface,
                             int iteration, float band_size, int height, int width)

{
  int err;
  extern PICINFO thePInfo;
  FILE *fopen(),*f1;
  char out[128]="";
  
  strcpy(out,outimage);
      
  /* save the bubbles */
  if ((iteration%how_often_save == 0) || (iteration == 1)) { 
   /* get_level_set (float *surface_array, float *level_set_array, float level, 
     int boundary_value,  int interpolate, int band_size,int height, int width); */
    get_level_set(surface_array, level_set_array,0,0,1,band_size,height,width); 
    CopyZLevelToUByte(level_set_array,thePInfo.pic,(height-2),(width-2));
    
    /* printf("the color type is:%d\n",thePInfo.colType); */                     
    sprintf(outimage,"%s-%1.2f-%1.2f-%04d.ras",out,alpha,beta,
            iteration);
    /*        printf("saving the raster image to: %s\n",outimage);  */
    err = ImageIOWrite(outimage,&thePInfo,F_SUNRAS,F_BWDITHER);
    if (err == writeFailure) {
      fprintf(stderr,"unable to write out the results to file %s.\nD'oooh!\n",
              outimage);
      exit(1);  
    } /* if err = write .... */
  }
 
  if ((save_surface != 0) && (iteration%save_surface ==0 || iteration==1)) {
    sprintf(outimage,"%s-%1.2f-%1.2f-%04d.raw",out,alpha,beta,
                iteration);
    printf("saving the raw image to: %s\n",outimage);  
    f1 = fopen(outimage,"w");
    if (f1 == 0) { printf("no such directory\n"); exit(5); }
    fwrite(surface_array, sizeof(float), (height*width), f1);
    fclose(f1);
  }
 
  strcpy(outimage,out);

}

void save_2d_RD_bubbles_results(char *outimage, float *surface_array, float *level_set_array, 
                                int *region, float alpha, float beta, int how_often_save,
                                int save_surface,int iteration, float band_size, 
                                int height, int width)

{
  
  int err,max,min;
  int y,x,pos;
  extern PICINFO thePInfo;
  extern PgmImage pgmim;
  FILE *fopen(),*f1;
  char out[128]="";
  
  strcpy(out,outimage);
      
  /* save the bubbles */
  if ((iteration%how_often_save == 0) || (iteration == 1)) { 
   /* get_level_set (float *surface_array, float *level_set_array, float level, 
     int boundary_value,  int interpolate, int band_size,int height, int width); */
    get_level_set(surface_array, level_set_array,0,0,1,band_size,height,width); 
    CopyZLevelToUByte(level_set_array,thePInfo.pic,(height-2),(width-2));
    
    /* printf("the color type is:%d\n",thePInfo.colType); */                     
    sprintf(outimage,"%s-%1.2f-%1.2f-%04d.ras",out,alpha,beta,
            iteration);
    /*        printf("saving the raster image to: %s\n",outimage);  */
    err = ImageIOWrite(outimage,&thePInfo,F_SUNRAS,F_BWDITHER);
    if (err == writeFailure) {
      fprintf(stderr,"unable to write out the results to file %s.\nD'oooh!\n",
              outimage);
      exit(1);  
    } /* if err = write .... */
  }
 
  if ((save_surface != 0) && (iteration%save_surface ==0 || iteration==1)) {
    sprintf(outimage,"%s-%1.2f-%1.2f-%04d.raw",out,alpha,beta,
                iteration);
    printf("saving the raw image to: %s\n",outimage);  
    f1 = fopen(outimage,"w");
    if (f1 == 0) { printf("no such directory\n"); exit(5); }
    fwrite(surface_array, sizeof(float), (height*width), f1);
    fclose(f1);
  }
  
  if ((iteration%how_often_save == 0) || (iteration == 1)) { 
  
    sprintf(outimage,"%s-%1.2f-%1.2f-%04d.pgm",out,alpha,beta,
                iteration);

    image_extremas_int(region, &max, &min, height, width);
    if ((max +5 ) > 255) 
      max = 255;

    pgmim.h = height;
    pgmim.w = width;
    if (pgmim.im == NULL) {
      pgmim.im = (unsigned char *) calloc(pgmim.w*pgmim.h,sizeof(unsigned char));
    }
    
    for(y=0; y<height; y++) {
      for(x=0; x<width; x++) {
        pos = y*width+x;
        if (region[pos] < 0.0) 
          pgmim.im[pos] = max+5;
        else 
          pgmim.im[pos] = region[pos];
      }
    }
    /* image_scale_UChar(&pgmim, height, width); */
    write_pgmimage(outimage,&pgmim); 
  }

  strcpy(outimage,out);
}








