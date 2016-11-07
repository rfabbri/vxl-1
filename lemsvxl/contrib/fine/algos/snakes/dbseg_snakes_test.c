#include <math.h>
#include <stdio.h>

#include <basic_utils.h>
#include <shock_tracer.h>
#include <pgm.h>
#include <contour_routines.h>

#include "snakes.h"
#include "matrix_lib.h"
#include "stop.h"
#include "utils.h"


PgmImage pgmim;
Tracer tracedshocks;

void main(int argc, char **argv)

{
  int i,ii,jj,kk,j,y,x,size,pos;
  int contour_type,length_of_contour,snake_length;
  int *image_array, *contour_array;
  InitPars par;
  Snake Sn;
  float x0,x1,xn1,xn,y0,y1,yn1,yn;
  float euclidean_dist;
  double **A, **Ainv;
  FILE *fopen(), *f1;
  float snake_size[1];
  char out[128] = "";
  struct window
  {
    int x0, y0;
    int x1, y1;
  }my_window;  
  
  snake_length = 2000;
  Sn.w1 = 0.3;
  Sn.w2 = 0.3;
  Sn.dt = 1.0;
  Sn.dt = 0.1;
  par.steps =1;
  par.how_often_save = 1;
  Sn.smoothing = 1.0;

  InitPgm(&pgmim);
  
  if (argc < 2) {   /* at least two arguments have to be inputed */ 
    usage_snakes();
    exit(11);
  }
  par.inimage = argv[1];
  par.snimage = argv[2]; 
  par.outimage = argv[3]; 
  
  strcpy(out,par.outimage);

  for ( --argc, ++argv; argc; --argc, ++argv ) {
    
    if ( **argv == '-' ) {  
      ++*argv;
      --argc;
      
      if ( keymatch( *argv, "w1", 2))
        Sn.w1 = atoi( *++argv);
      else if ( keymatch ( *argv, "w2", 2))
        Sn.w2 = (atof( *++argv));
      else if ( keymatch ( *argv, "smoothing", 9))
       Sn.smoothing = atof( *++argv);
      else if ( keymatch ( *argv, "steps", 5))
        par.steps = atoi( *++argv);
      else if ( keymatch ( *argv, "delta_t", 7))
        Sn.dt = atoi( *++argv);
      else if ( keymatch ( *argv, "how_often_save", 14))
        par.how_often_save = atoi( *++argv);
    }
    
  } /* end if else */
  
  
  if ((par.inimage == NULL) || (par.outimage == NULL))  {
    usage_snakes();
    exit(11);
  }
  
  read_pgmimage(par.inimage,&pgmim);
  
  Sn.h = pgmim.h;  
  Sn.w  = pgmim.w;
  
  printf("h=%i and w=%i and size =%i and w1 =%f and w2 =%f\n", 
         pgmim.h,pgmim.w,Sn.size,Sn.w1,Sn.w2);
  
  allocate_memory_snakes(&Sn);
  image_array  = (int *) calloc(Sn.h*Sn.w,sizeof(int));
  contour_array = (int *) calloc(Sn.h*Sn.w,sizeof(int));
  
  printf("reading the image\n");
  CopyIm_UCharToFloat(pgmim.im, Sn.image,Sn.h,Sn.w); 

  f1 = fopen(par.outimage, "w");
  fwrite(Sn.image, sizeof(float), (Sn.h*Sn.w), f1);
  fclose(f1);

  printf("reading the snakes\n");
  read_pgmimage(par.snimage,&pgmim);
  CopyIm_UCharToInt(pgmim.im, image_array,Sn.h,Sn.w); 

  my_window.x0 = 0;
  my_window.y0 = 0;
  my_window.x1 = Sn.w-1;
  my_window.y1 = Sn.h-1;
 
  length_of_contour = Sn.h*Sn.w;
  
  /* trace the contours */
  find_start (0, contour_array, length_of_contour, image_array, (Sn.w-1),(Sn.h-1));

  Sn.vec.x = (float *) calloc(snake_length,sizeof(float));
  Sn.vec.y = (float *) calloc(snake_length,sizeof(float));
  Sn.temp.x = (float *) calloc(snake_length,sizeof(float));
  Sn.temp.y = (float *) calloc(snake_length,sizeof(float));
  

  ii=0;
  while(contour_array[ii] != ENDOFCHAINLIST) {
    jj=0;
    while(contour_array[ii] !=CHAINEND) {
      Sn.vec.x[jj] = (float) contour_array[ii];
      Sn.vec.y[jj] = (float) contour_array[ii+1];
      printf("y=%i and x =%i\n", (int) Sn.vec.y[jj], (int) Sn.vec.x[jj]);
      jj++; ii+=2;
    }
    Sn.size = jj-1;
    printf("moving snakes: iteration =%i\n", 0);
    snake_size[0] = (float) Sn.size;
    sprintf(par.outimage,"%s-%03d.raw",out,0);
    f1 = fopen(par.outimage,"w");
    fwrite(snake_size,sizeof(float),1,f1);
    fwrite(Sn.vec.y,sizeof(float),(Sn.size),f1);
    fwrite(Sn.vec.x,sizeof(float),(Sn.size),f1);
    fclose(f1);

    run_snakes(&Sn, par.steps);

    printf("moving snakes: iteration =%i\n", par.steps);
    snake_size[0] = (float) Sn.size;
    sprintf(par.outimage,"%s-%03d.raw",out,par.steps);
    f1 = fopen(par.outimage,"w");
    fwrite(snake_size,sizeof(float),1,f1);
    fwrite(Sn.vec.y,sizeof(float),(Sn.size),f1);
    fwrite(Sn.vec.x,sizeof(float),(Sn.size),f1);
    fclose(f1);
    ii++;
    break;
  }
  free_memory_snakes(&Sn);
  free(image_array);
  free(contour_array);

}


  
   











