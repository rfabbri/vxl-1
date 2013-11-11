#include "subpixel_bubbles.h"

void test_eno_zerox_boundary_fill(int argc, char *argv[])
{
   FILE *fp;
   double *surface;
   double *fsurface;
   int height, width;
   ENO_Interval_Image_t *enoimage;
   char *filename;
   int *label;
   int x,y,i;
   int xx,yy,cnt=0;

   if (argc != 6){
     fprintf(stderr,"Usage: test_fill file_name height width y x\n");
     exit(1);
   }

   filename=argv[1];
   height = atoi(argv[2]);
   width = atoi(argv[3]);
   y=atoi(argv[4]);
   x=atoi(argv[5]);

   label=(int *)calloc(height*width,sizeof(int));
   surface=(double *)calloc(height*width,sizeof(double));
   fsurface=(double *)calloc(height*width,sizeof(double));

   if (!surface){ 
     printf("Mem. Alloc fault");
     exit(1);
   }

   fp=fopen(filename,"r");
   if (fp == NULL){
     printf("File opening error");
     exit(1);
   }

   printf("%d\n",fread(fsurface,sizeof(double),height*width,fp));
   for (i=0;i<height*width;i++)
     surface[i]=(double)fsurface[i];

   enoimage = enoimage_from_data_tbs(surface,height,width);
   printf("H=%d W=%d\n",enoimage->height,enoimage->width);

   fill_regions(enoimage,label,fsurface);
   /*for (i=0;i<height*width;i++)
     if (fsurface[i] > 0)
     printf("good");*/
   
   /*  print_2d_int_array(label,height,width, stdout); */
   fp=fopen("region.dat","w");
   fwrite(label,sizeof(int),height*width,fp);
   fclose(fp);

   free(label);
   free(surface);
   free(fsurface);
   enoimage_free(enoimage); 
}


/* void test_inter_bubble_shocks(int argc, char *argv[])
{

  SP_Bubble_Deform spbub_deform;
  Tracer Tr;
  FILE *fp;
  int i,j;

  convert_boundary_to_linear(spbub_deform.cedt.boundary,
  spbub_deform.linear_boundary);
  inter_boundary_shocks(spbub_deform.linear_boundary,
  &spbub_deform.shocks,50,50);

}*/

void main(int argc, char *argv[])
{
  test_eno_zerox_boundary_fill(argc, argv);
}




