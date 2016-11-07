/*------------------------------------------------------------*\
 * File: pgm.c
 *
 * Functions:
 *  read_pgmimage
 *  write_pgmimage
 *  InitPgm
 *
 * History:
 *  #0:  by H.Tek, Jan 30, 1997
\*------------------------------------------------------------*/

#include <stdio.h>
#include <vcl_cmath.h>
#include <stdlib.h>
#include <string.h>

#include "pgm.h"

/*------------------------------------------------------------*\
 * Function: read_pgmimage
 *
 * Usage:  Loads PGM image (2D)
\*------------------------------------------------------------*/
void
read_pgmimage(char *file, PgmImage *pgmim)

{
  FILE  *f1; //*fopen(),
  char jnk[128];
  int imsize,prev_imsize;

  prev_imsize = pgmim->h*pgmim->w;

  f1 = fopen(file, "r");
  if (f1== NULL) {
    printf("file %s not found\n", file);
    exit(1);
  }

  fscanf(f1,"%s\n", jnk);
  fscanf(f1,"%s", jnk);
  if (strcmp(jnk,"#\n")) {
    char jj;
    do jj= fgetc(f1);
     while (jj!='\n');
    
  }

  fscanf(f1,"%i %i\n", &pgmim->w, &pgmim->h);
  fscanf(f1,"%i\n", &pgmim->maxval);

  printf("height =%i and width =%i and maxval =%i\n", pgmim->h,pgmim->w,pgmim->maxval);
  
  imsize = pgmim->h*pgmim->w;

  if (pgmim->im == NULL) {
    pgmim->im = (unsigned char *) calloc(imsize,sizeof(unsigned char));
  }
  else if (imsize != prev_imsize)
    pgmim->im = (unsigned char *) realloc(pgmim->im,imsize*sizeof(unsigned char));
  
  fread(pgmim->im, sizeof(unsigned char),imsize,f1);
  fclose(f1);
}

/*------------------------------------------------------------*\
 * Function: write_pgmimage
 *
 * Usage:  Saves PGM image (2D)
\*------------------------------------------------------------*/
void
write_pgmimage(char *file, PgmImage *pgmim)
{
  FILE  *f1; //*fopen(),
  
  f1 = fopen(file, "w");
  if (f1== NULL) {
    printf("file %s not found\n", file);
    exit(1);
  }
  if (pgmim->im == NULL) {
    pgmim->im = (unsigned char *) calloc(pgmim->w*pgmim->h,sizeof(unsigned char));
  }

  fprintf(f1,"P5\n");
  fprintf(f1,"%i %i\n",pgmim->w,pgmim->h);
  fprintf(f1,"%i\n",pgmim->maxval);
  fwrite(pgmim->im,sizeof(unsigned char), (pgmim->h*pgmim->w),f1);
  fclose(f1);  
}

/*------------------------------------------------------------*\
 * Function: InitPgm
 *
 * Usage:  Inits PGM data structure
\*------------------------------------------------------------*/
void
InitPgm(PgmImage *pgmim)
{
  pgmim->im = NULL;
  pgmim->h = 0;
  pgmim->w = 0;
  pgmim->maxval = 255;
  pgmim->type = "P5";
}

/* ================================================ */
