
#include <math.h>
#include <stdio.h>

#include <basic_utils.h>
#include <shock_tracer.h>
#include <pgm.h>

#include "snakes.h"
#include "matrix_lib.h"
#include "stop.h"
#include "utils.h"


PgmImage pgmim;
Tracer tracedshocks;

void main(int argc, char **argv)

{
  int i,ii,jj,kk,j,y,x,size,pos;
  int contour_type;
  InitPars par;
  Snake Sn;
  ContourMap *Cmap;
  double **A, **Ainv;
  FILE *fopen(), *f1;
  float *tempy,*tempx;
  float *temp;
  float x0,xn,y0,yn;
  float x1,xn1,y1,yn1;
  float euclidean_dist;
  float snake_size[1];
  char out[128] = "";

  Sn.w1 = 0.3;
  Sn.w2 = 0.3;
  Sn.dt = 1.0;
  Sn.size = 130;
  Sn.dt = 0.1;

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
      
      if ( keymatch( *argv, "w1", 5))
        Sn.w1 = atoi( *++argv);
      else if ( keymatch ( *argv, "w2", 4))
        Sn.w2 = (atof( *++argv));
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
  
  Sn.h = pgmim.h+2;  
  Sn.w  = pgmim.w+2;
  
  Sn.h = pgmim.h;
  Sn.w = pgmim.w;

  printf("h=%i and w=%i and size =%i and w1 =%f and w2 =%f\n", 
         Sn.h,Sn.w,Sn.size,Sn.w1,Sn.w2);
  
  allocate_memory_snakes(&Sn);
  temp = (float *) calloc(Sn.h*Sn.w,sizeof(float));
  
  printf("reading the image\n");
  CopyIm_UCharToFloat(pgmim.im, Sn.image,Sn.h,Sn.w); 
  
  printf("reading the snakes\n");
  read_pgmimage(par.snimage,&pgmim);
  CopyIm_UCharToFloat(pgmim.im, temp,Sn.h,Sn.w); 
 
  compute_image_forces (Sn.image, Sn.stop, Sn.fx, Sn.fy, 2.0,Sn.h,Sn.w);


  Cmap = (ContourMap *) calloc(100,sizeof(ContourMap));
  printf("uses shocks for snakes\n");
  find_all_shocks_as_snakes(temp, &tracedshocks, Cmap, Sn.h, Sn.w);

  printf("test =%i\n", Cmap[1].no);
  
  for(ii=1; ii<Cmap[1].no; ii++) {
    printf("label =%i and size =%i\n", ii, Cmap[ii].size);
    printf("start: type =%i and y=%f and x =%f, pos =%i\n",  Cmap[ii].start.type,
           Cmap[ii].start.y, Cmap[ii].start.x, Cmap[ii].start_pos);
    printf("end: type =%i and y=%f and x =%f, pos =%i\n",  Cmap[ii].end.type,
           Cmap[ii].end.y, Cmap[ii].end.x, Cmap[ii].end_pos);
  } 
  
  Sn.vec.x = (float *) calloc(tracedshocks.size,sizeof(float));
  Sn.vec.y = (float *) calloc(tracedshocks.size,sizeof(float));
  tempx = (float *) calloc(tracedshocks.size,sizeof(float));
  tempy = (float *) calloc(tracedshocks.size,sizeof(float));
  
  printf("size =%i\n", tracedshocks.size);

  ii=0;
  while(tracedshocks.vert[ii] != ENDOFLIST) {
    jj=0; kk=ii;
    while(tracedshocks.vert[ii] !=ENDOFCONTOUR) {
      ii++; jj++;  
    }
    Sn.size = jj-1;
    printf("allocating %i memory  for A and Ainv\n", Sn.size);
    A = MAXgen(Sn.size, Sn.size);
    Ainv = MAXgen(Sn.size, Sn.size);
    ii = kk; jj=0;
    while (tracedshocks.vert[ii] != ENDOFCONTOUR) {
      Sn.vec.y[jj] = tracedshocks.vert[ii];
      Sn.vec.x[jj] = tracedshocks.horiz[ii];
      printf("ii=%i and shocks = %f and %f\n",ii, tracedshocks.vert[ii], 
             tracedshocks.horiz[ii]);
      ii++; jj++;
    }

    y0 = Sn.vec.y[0];
    yn = Sn.vec.y[Sn.size-1];
    x0 = Sn.vec.x[0];
    xn = Sn.vec.x[Sn.size-1];
    y1 = Sn.vec.y[1];
    yn1 = Sn.vec.y[Sn.size-2];
    x1 = Sn.vec.x[1];
    xn1 = Sn.vec.x[Sn.size-2];

    euclidean_dist = sqrt((y0-yn)*(y0-yn)+(x0-xn)*(x0-xn));
 
    printf("euclidean dist = %f\n", euclidean_dist);

    if (euclidean_dist < 1.1) 
      contour_type = CLOSECONTOUR;
    else
      contour_type = OPENCONTOUR;

    find_matrix_coeffiecents(A, Sn.w1, Sn.w2, Sn.size, contour_type);
    
    for(ii=0; ii<Sn.size; ii++) {
      for(jj=0; jj<Sn.size; jj++) {
        if (ii==jj) 
          A[ii][jj] = 1.0 + Sn.dt*A[ii][jj];
        else 
         A[ii][jj] = Sn.dt*A[ii][jj]; 
      }
    }
    printf("Ainv = A-1\n");
    MAXinv(A, Sn.size, Ainv); 
    for(ii=0; ii<=par.steps; ii++) {
      /* goto out; */
      for(i=0; i<Sn.size; i++) {
        tempx[i] =0.0; tempy[i] = 0.0;
        for(j=0; j<Sn.size; j++) {
          pos = (int) Sn.vec.y[j]*Sn.w+(int)Sn.vec.x[j];
          tempy[i] += (Ainv[i][j]*(Sn.vec.y[j]-Sn.fy[pos]));
          tempx[i] += (Ainv[i][j]*(Sn.vec.x[j]-Sn.fx[pos]));
        }
      }
      for(i=0; i<Sn.size; i++) {
        if (tempy[i] >= 0.0 && tempy[i] < Sn.h) 
          Sn.vec.y[i] = tempy[i];
        if (tempx[i] >= 0.0 && tempx[i] < Sn.w)
          Sn.vec.x[i] = tempx[i];
      }

#ifdef TEST
      if (contour_type == OPENCONTOUR) {
        Sn.vec.y[0] = y0;
        Sn.vec.y[Sn.size-1] = yn;
        Sn.vec.x[0] = x0;
        Sn.vec.x[Sn.size-1] = xn;
        Sn.vec.y[1] = y1;
        Sn.vec.y[Sn.size-2] = yn1;
        Sn.vec.x[1] = x1;
        Sn.vec.x[Sn.size-2] = xn1;
      }
#endif TEST

      if (ii%par.how_often_save == 0 || ii==0) {
        printf("moving snakes: iteration =%i\n", ii);
        snake_size[0] = (float) Sn.size;
        sprintf(par.outimage,"%s-%03d.raw",out,ii);
        f1 = fopen(par.outimage,"w");
        fwrite(snake_size,sizeof(float),1,f1);
        fwrite(Sn.vec.y,sizeof(float),(Sn.size),f1);
        fwrite(Sn.vec.x,sizeof(float),(Sn.size),f1);
        fclose(f1);
      }
    }
    break;
    ii++;

  }
  free_memory_snakes(&Sn);
  free(temp);
}


  
   









