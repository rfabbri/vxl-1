#include <math.h>
#include <stdio.h>
#include <basic_utils.h>

#include "dbseg_snakes_utils.h"
#include "dbseg_snakes_snakes.h"
#include "dbseg_snakes_matrix_lib.h"
#include "dbseg_snakes_stop.h"

void run_snakes(Snake *Sn, int steps)

{
  int i,ii,jj,kk,j,y,x,size,pos;
  int contour_type;
  double **A, **Ainv;
  FILE *fopen(), *f1;
  float x0,xn,y0,yn;
  float x1,xn1,y1,yn1;
  float euclidean_dist;
  float snake_size[1];
  
  compute_image_forces (Sn->image, Sn->stop, Sn->fx, Sn->fy, Sn->smoothing,Sn->h,Sn->w);

  A = MAXgen(Sn->size, Sn->size);
  Ainv = MAXgen(Sn->size, Sn->size);
  y0 = Sn->vec.y[0];
  yn = Sn->vec.y[Sn->size-1];
  x0 = Sn->vec.x[0];
  xn = Sn->vec.x[Sn->size-1];
  y1 = Sn->vec.y[1];
  yn1 = Sn->vec.y[Sn->size-2];
  x1 = Sn->vec.x[1];
  xn1 = Sn->vec.x[Sn->size-2];
  
  euclidean_dist = sqrt((y0-yn)*(y0-yn)+(x0-xn)*(x0-xn));
  
  printf("euclidean dist = %f\n", euclidean_dist);
  
  if (euclidean_dist < 3.1) 
    contour_type = CLOSECONTOUR;
  else
    contour_type = OPENCONTOUR;
  
  find_matrix_coeffiecents(A, Sn->w1, Sn->w2, Sn->size, contour_type);
  
  for(ii=0; ii<Sn->size; ii++) {
    for(jj=0; jj<Sn->size; jj++) {
      if (ii==jj) 
        A[ii][jj] = 1.0 + Sn->dt*A[ii][jj];
      else 
        A[ii][jj] = Sn->dt*A[ii][jj]; 
    }
  }
  printf("Ainv = A-1\n");
  MAXinv(A, Sn->size, Ainv); 

  for(ii=1; ii<=steps; ii++) {
    /* goto out; */
    for(i=0; i<Sn->size; i++) {
      Sn->temp.x[i] =0.0; Sn->temp.y[i] = 0.0;  
      for(j=0; j<Sn->size; j++) {
        pos = (int) Sn->vec.y[j]*Sn->w+(int)Sn->vec.x[j];
        Sn->temp.y[i] += (Ainv[i][j]*(Sn->vec.y[j]-Sn->fy[pos]));
        Sn->temp.x[i] += (Ainv[i][j]*(Sn->vec.x[j]-Sn->fx[pos]));
      }
    }
    for(i=0; i<Sn->size; i++) {
      if (Sn->temp.y[i] >= 0.0 && Sn->temp.y[i] < Sn->h) 
        Sn->vec.y[i] = Sn->temp.y[i];
      if (Sn->temp.x[i] >= 0.0 && Sn->temp.x[i] < Sn->w)
        Sn->vec.x[i] = Sn->temp.x[i];
    }
    /*  printf("iteration =%i\n", ii); */
#ifdef TEST
    if (contour_type == OPENCONTOUR) {
      Sn->vec.y[0] = y0;
      Sn->vec.y[Sn->size-1] = yn;
      Sn->vec.x[0] = x0;
      Sn->vec.x[Sn->size-1] = xn;
      Sn->vec.y[1] = y1;
      Sn->vec.y[Sn->size-2] = yn1;
      Sn->vec.x[1] = x1;
      Sn->vec.x[Sn->size-2] = xn1;
    }
#endif 
  }
  
}


  
   











