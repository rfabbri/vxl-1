#include <postscript.h>

#include "subpixel_bubbles.h"


void main(int argc, char *argv[])
{

  
  SP_Bubble_Deform spbub_deform;
  FILE *fp;
  int i,j;
  int no_of_iter;
  int height=10,width=10;
  unsigned char *seed;
  char *img_fname, *seed_fname;
  double *image;
  
  if (argc != 6){
    printf("Usage: test_bub image seed height width no_of_iter\n");
    exit(1);
  }
  
  img_fname = (char *)malloc(sizeof(char)*200);
  seed_fname = (char *)malloc(sizeof(char)*200);
  fp = (FILE *)malloc(sizeof(FILE));

  i=1;
  strcpy(img_fname,argv[i]);i++;
  strcpy(seed_fname,argv[i]);i++;
  height = atoi(argv[i]);i++;
  width = atoi(argv[i]);i++;
  no_of_iter =  atoi(argv[i]);i++;

  image =  (double *)calloc(height*width,sizeof(double));
  seed =  (unsigned char *)calloc(height*width,sizeof(unsigned char));
  fp=fopen(img_fname,"r");
  fread(image,sizeof(double),height*width,fp);
  fclose(fp);
  /*fp=fopen("img.dat","w");
  fwrite(image,sizeof(double),height*width,fp);
  fclose(fp);*/

  fp=fopen(seed_fname,"r");
  fread(seed,sizeof(unsigned char),height*width,fp);
  fclose(fp);
  /*fp=fopen("seed.dat","w");
  fwrite(seed,sizeof(unsigned char),height*width,fp);
  fclose(fp);
*/


  initialize_sp_bub_deform(image,seed,height,width,0,no_of_iter,-0.1,0.0,&spbub_deform);

  for (i=0;i<spbub_deform.bubbles.no_of_regions;i++)
    print_region_stats(&spbub_deform.bubbles.region[i]);
 
   subpixel_bubble_evolution(&spbub_deform); 
  
  /*fp=fopen("dist.raw","w");
  fwrite(spbub_deform.bubbles.surface,sizeof(double),
         spbub_deform.height*spbub_deform.width,fp);
  fclose(fp);
*/

  /*printf("Image\n");
  print_2d_double_array(spbub_deform.image,spbub_deform.height,spbub_deform.width,stdout);
  printf("Seed\n");
  print_2d_uchar_array(spbub_deform.seed,spbub_deform.height,spbub_deform.width,stdout);
   printf("Region\n");
  print_2d_int_array(spbub_deform.bubbles.region_label,spbub_deform.height,spbub_deform.width,stdout);*/
 

  free(seed);
  free(image);
  free_sp_bub_deform(&spbub_deform);
  free(img_fname);
  free(seed_fname);
}






