#ifndef _SNAKE_TYPES_H_
#define _SNAKE_TYPES_H_

#define CLOSECONTOUR -1.0
#define OPENCONTOUR -2.0

typedef struct {
  int steps, how_often_save;
  char *inimage, *outimage, *snimage;
}InitPars;

typedef struct {
  float *x,*y;
  int size;
} Vec;



typedef struct {
  int h,w,size;
  float w1,w2,smoothing,dt;
  float *image;
  float *stop;
  float *fx, *fy;
  Vec vec, temp;
}Snake;

void run_snakes(Snake *Sn, int steps);

#endif /* SNAKE_TYPES */


