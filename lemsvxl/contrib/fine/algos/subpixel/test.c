#include <postscript.h>

#include "subpixel_bubbles.h"


void main(int argc, char *argv[])
{

  
  Region_Stats region[3];
  Window_Stats win;
  ENO_Interval_Image_t *enoimage;
  Point grid_point,boundary_point;
  FILE *fp;
  int i,j;
  int height=10,width=10;
  float *image;
  float first_der, interp_value;
  double *dimage;

  allocate_window_stats(&win,height,width);
  image =  (float *)calloc(height*width,sizeof(float));
  dimage =  (double *)calloc(height*width,sizeof(double));
  fp=fopen("test-surf-10x10.dat","r");
  fread(image,sizeof(float),height*width,fp);
  fclose(fp);

  CopyIm_FloatToDouble(image,dimage,height,width);

  enoimage=enoimage_from_data(dimage,height,width);
 
  grid_point.x=5.0;
  grid_point.y=5.0;
  boundary_point.x=atof(argv[1]);
  boundary_point.y=atof(argv[2]);
 
  compute_subpixel_interp_1der(grid_point,boundary_point,enoimage, 
                   &first_der, &interp_value);
  printf("Der=%f Val=%f\n",first_der,interp_value);

  compute_square_window_stats(image,&win,height,width,3);
  /*printf("Image\n");
    print_2d_float_array(image,height,width,stdout);
    printf("Mean\n");
    print_2d_float_array(win.mean,height,width,stdout);
    printf("SD\n");
    print_2d_float_array(win.stdev,height,width,stdout);
    printf("N\n");
    print_2d_int_array(win.no_of_pixels,height,width,stdout);*/

  enoimage_free(enoimage);
  free(image);
  free(dimage);

}




