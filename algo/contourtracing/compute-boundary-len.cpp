#include <stdio.h>
#include <stdlib.h>
#include <vcl_cmath.h>
#include "utils.h"
#include "redt.h"
#include "graphics.h"
#include "image_processing.h"
//#include "postscript.h"

/* Writes the Tracer data directly from a file */
// write the ContourPoint* _ptListHead
void writeTracerToFile2(char *contour_file_name, int height, int width,  Tracer *Tr)
{
  FILE *fp;
  double dcount,tmp;
  int i,j,ii,count,jj,kk,valid_cnt;
  double *arr;
  int contour_length[2550],verbose=1;
  int contour_label[2550];

  fp = fopen(contour_file_name, "w");
  if (fp==NULL) {
    fprintf(stderr, "Can't open file %s\n", contour_file_name);
    exit(1);
  }

  arr=(double*)malloc(5*(Tr->size+4)*sizeof(double));

  ii =0;
  count=0;kk=0;
  while(Tr->vert[ii] != ENDOFLIST) {
    jj=0;
    while(Tr->vert[ii] != ENDOFCONTOUR) {
      ii++;jj++;
    }
    if (jj > 1){
      ii=ii-jj;
      jj=0;
      while(Tr->vert[ii] != ENDOFCONTOUR) {
  arr[2*count]=Tr->horiz[ii];
  arr[2*count+1]=Tr->vert[ii];
  contour_label[kk]=Tr->label[ii];
  count++;
  ii++;jj++;
      }
      contour_length[kk]=jj;
      kk++;

    }
    /* printf("%d-%d-%f\n",ii,kk,Tr->vert[ii]); */
    ii++;
  }
  valid_cnt=kk;

  dcount=(double)count;
  if (verbose){
    printf("Found %d contours for  writing\n",valid_cnt);
    printf("Found %d valid contour point for  writing\n",count);
  }
  fwrite(&valid_cnt,sizeof(int),1,fp);
  for (i=0;i<kk;i++){
    fwrite(&contour_length[i],sizeof(int),1,fp);
    if (verbose){
      printf("Writing contour #%d of length %d and label=%d\n",i,
       contour_length[i],contour_label[i]);
    }
    if (contour_label[i] > 10){
      printf("Writing contour #%d of length %d and label=%d\n",i,
       contour_length[i],contour_label[i]);
    }
  }
  fwrite(arr,sizeof(double),2*count,fp);

  fclose(fp);
  free(arr);

}

/* void writeTracerToRaphCon(char *contour_file_name, int height, int width,  Tracer *Tr) */

int main(int argc, char * argv[])
{
  /* Chris Added: */
  FILE *point_file;
  int pointcnt=0;
  char strtemp1[100];
  char strtemp2[100];
  int pos=0, i=0;
  int outpoints=0;
  int tempmax=0;
  int max_contour_index = 0;
  int contour_counter=0;

  char input_file_name[200] = "bird10.pgm";
  char *first,*last;
  unsigned char *input_image;
  float *fsurface_array;
  double *surface_array;
  int height=0,width=0,size;
  int smooth=0;
  int ii,kk,jj,ll,count;
  Tracer trac,*Tr;
  PgmImage pgmim;
  FILE *fp;

  double dist;
  double *xarr,*yarr;
  double contour_arc_length[2550];
  int contour_length[2550];



  //if (argc < 2){
  //  fprintf(stderr,"%d\n",argc);
  //  fprintf(stderr,"Usage: computeBoundaryLength input-file -smooth smooth\n");
  //  exit(1);
  //}

  /*if (strcmp( argv[argc-1], "-points") == 0) {
    outpoints = 1;
    argc--;
  }

  strcpy(input_file_name,argv[1]);
  for (--argc, ++argv; argc; --argc, ++argv ) {
    if ( **argv == '-' ) {
      ++*argv;
      --argc;
      if (keymatch( *argv, "smooth", 6))
        smooth = atoi( *++argv);
    }
  }*/

  InitPgm(&pgmim);

  read_pgmimage(input_file_name,&pgmim);
  height = pgmim.h;
  width = pgmim.w;
  size = height*width;
  if (size == 0){
    fprintf(stderr,"Invalid Height or Width H=%d W=%d\n",height,width);
    exit(1);
  }

  input_image=pgmim.im;
  for (ii=0;ii<size;ii++)
    input_image[ii]=input_image[ii]*255;

  surface_array=(double *)calloc(size,sizeof(double));
  fsurface_array=(float *)calloc(size,sizeof(float));

  signed_distance_transform (input_image, fsurface_array,height, width);
  CopyIm_FloatToDouble(fsurface_array,surface_array,height,width);
  smooth=1;
  if (smooth>0)
    curvature_smoothing(surface_array,0.3,smooth,0, height,width);

  trace_eno_zero_xings(surface_array, &trac, height,  width);

  writeTracerToFile2("tracer.dat", height, width, &trac);

  Tr=&trac;
  xarr=(double*)malloc(5*(Tr->size+4)*sizeof(double));
  yarr=(double*)malloc(5*(Tr->size+4)*sizeof(double));
  ii =0;
  kk=0;
  while(Tr->vert[ii] != ENDOFLIST) {
    jj=0;
    while(Tr->vert[ii] != ENDOFCONTOUR) {
      ii++;jj++;
    }
    if (jj > 1){
      ii=ii-jj;
      jj=0;count=0;
      while(Tr->vert[ii] != ENDOFCONTOUR) {
  xarr[count]=Tr->horiz[ii];
  yarr[count]=Tr->vert[ii];
  count++;
  ii++;jj++;
      }
      contour_length[kk]=jj;
      dist=0;
      for (ll=1;ll<contour_length[kk];ll++){
  dist+=sqrt(pow(xarr[ll]-xarr[ll-1],2.0)+pow(yarr[ll]-yarr[ll-1],2.0));
      }
      contour_arc_length[kk]=dist;
      kk++;
    }
    /* printf("%d-%d-%f\n",ii,kk,Tr->vert[ii]); */
    ii++;

    return 0;
  }





 /* Chris Changed:  8/10/00 */

  for (i=0;i<kk;i++){
    printf(" CONTOUR LENGTH %d= %d \n", i, contour_length[i]);
    if  ( contour_length[i] > tempmax ) {
      tempmax = contour_length[i];
      max_contour_index = i;
    }
  }

  printf(" the max contour is #%d and it has %d elements\n", max_contour_index,
   contour_length[max_contour_index]);

  if ( outpoints = 1 ) {


    strcpy(strtemp1, input_file_name);
    for ( i=0; strtemp1[i] != '.'; i++ ){}
    strtemp1[i]= 0;
    sprintf(strtemp2, "%s-points.con", strtemp1);
    printf( "\n\nCreated %s\n", strtemp2);

    point_file = fopen(strtemp2, "w");

    fprintf(point_file, "CONTOUR\nCLOSE\n%d\n", contour_length[max_contour_index]);
    pos = 0;

    Tr=&trac;
    xarr=(double*)malloc(5*(Tr->size+4)*sizeof(double));
    yarr=(double*)malloc(5*(Tr->size+4)*sizeof(double));
    ii =0;
    kk=0;
    while(Tr->vert[ii] != ENDOFLIST) {
      jj=0;
      while(Tr->vert[ii] != ENDOFCONTOUR) {
  ii++;jj++;
      }
      if (jj > 1 && contour_counter == max_contour_index){
  ii=ii-jj;
  jj=0;count=0;
  while(Tr->vert[ii] != ENDOFCONTOUR) {
    fprintf(point_file, "%f %f ", Tr->horiz[ii], Tr->vert[ii]);
    count++;
    ii++;jj++;
  }
  kk++;
      }
      /* printf("%d-%d-%f\n",ii,kk,Tr->vert[ii]); */
      contour_counter++;
      ii++;
    }
    fclose(point_file);

  }
    /* end of changes: */


  dist=0;
  for (ll=0;ll<kk;ll++){
    /////printf("%2d %3d %5.2f\n",ll,contour_length[ll],contour_arc_length[ll]);
    if (contour_arc_length[ll]>dist)
      dist=contour_arc_length[ll];
  }

  fp=fopen("arc-lengths.txt","a");

  first=strrchr(input_file_name,'/');
  if (first == NULL)
    first=input_file_name;
  else
    first++;
  last=strrchr(input_file_name,'.');
  /*fprintf(fp,"%c",first);*/
  while(first!=last){
    fprintf(fp,"%c",*first);
    first++;
  }
  fprintf(fp," %8.4f\n",dist);
  fclose(fp);


  free(xarr);
  free(yarr);
  free_data_tracer(&trac);
  free(surface_array);
  free(fsurface_array);

}


