//ContourTracing.cpp

#include <stdio.h>
#include <stdlib.h>
#include <vcl_cmath.h>
#include "utils.h"
#include "redt.h"
#include "graphics.h"
#include "image_processing.h"
//#include "postscript.h"
#include "ContourTracing.h"

#include <vcl_iostream.h>
#include <vil/vil_image_view.h>

/* Writes the Tracer data directly from a file */
void writeTracerToFile2(char *contour_file_name, int height, int width,  Tracer *Tr)
{
  FILE *fp;
  double dcount;
  int i,ii,count,jj,kk,valid_cnt;
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

//input: unsigned char** img_, int width, int height
//\return number of points traced.
int contour_tracing (unsigned char* _img, int width, int height, double _sigma, 
                     vcl_vector<vsol_point_2d_sptr>& points, char* input_file_name)
{

  // Chris Added: 
  FILE *point_file;
  int pointcnt=0;
  char strtemp1[100];
  char strtemp2[100];
  int pos=0, i=0;
  int outpoints=0; //:< for debugging
  int tempmax=0;
  int max_contour_index = 0;
  int contour_counter=0;

  unsigned char *input_image = _img;
  float *fsurface_array;
  double *surface_array;
  int smooth=0;
  int ii,kk,jj,ll,count;
  Tracer trac,*Tr;

  double dist;
  double *xarr,*yarr;
  double contour_arc_length[2550];
  int contour_length[2550];
  
  int size = height*width;
  if (size == 0){
    vcl_cout<< "Invalid Width:"<<width<< " or Height: "<<height<< vcl_endl;
    return 0;
  }

  ///  input_image=_img->grayData();
  //for (ii=0;ii<size;ii++)
  //  input_image[ii]=input_image[ii]*255;

  surface_array =(double *)calloc(size,sizeof(double));
  fsurface_array=(float  *)calloc(size,sizeof(float));

  signed_distance_transform (input_image, fsurface_array,height, width);
  CopyIm_FloatToDouble(fsurface_array,surface_array,height,width);
  smooth=1;
  if (smooth>0)
    curvature_smoothing(surface_array,0.3,smooth,0, height,width);

  int _num_of_contours = trace_eno_zero_xings(surface_array, &trac, height,  width);


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
        dist += hypot (xarr[ll]-xarr[ll-1],yarr[ll]-yarr[ll-1]);
      }
      contour_arc_length[kk]=dist;
      kk++;
    }
    ii++;
  }



 // Chris Changed:  8/10/00 *

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

    //ContourPoint* _cur;
    vsol_point_2d_sptr point;

    //
    //  contour_length[i]


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

          //Point2D<double> _pt (Tr->horiz[ii], Tr->vert[ii]);
          double x = Tr->horiz[ii];
          double y = Tr->vert[ii];
          point = new vsol_point_2d (x, y);
          points.push_back (point);
          //ContourPoint* _new = new ContourPoint (_pt, 0, NULL, NULL);
          //if (*_ptListHead==NULL) {
          //  *_ptListHead = _new;
          //  _cur = _new;
          //}
          //else {
          //  _cur->SetNextPoint (_new);
          //  _new->SetPrevPoint (_cur);
          //  _cur = _new;
          //}

          count++;
          ii++;jj++;
        }
        kk++;
      }
      // printf("%d-%d-%f\n",ii,kk,Tr->vert[ii]);
      contour_counter++;
      ii++;
    }
    fclose(point_file);

  }
    // end of changes:


  free(xarr);
  free(yarr);
  free_data_tracer(&trac);
  free(surface_array);
  free(fsurface_array);

  return contour_length[max_contour_index];
}

// Main tracer routine
//: return number of points
int ContourTracing::
detectContour (void)
{
  assert (image_.ptr()!=NULL);

  //Construct the image array for contour tracer, from vil_image_resource_sptr
  vil_image_resource_sptr image_sptr = image_;
  vil_image_view< unsigned char > image_view = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );

  //alloc the buffer
  int width  = image_sptr->ni();
  int height = image_sptr->nj();
  int size   = width*height;
  unsigned char* gray_data = new unsigned char[size];
  //1D buffer: pixel(x,y) = gray_data[y*width+x]

  // threshold
  for (int x=0 ; x<image_sptr->ni(); x++) {
    for (int y=0 ; y<image_sptr->nj(); y++) {
      float sum = 0;
      for (int p=0 ; p<image_sptr->nplanes(); p++) { //p=3 for color image
        sum += image_view(x,y,p);
      }
      //GrayScale Value = 0.3*R + 0.59*G + 0.11*B
      //gray_data[y*width+x] = (unsigned char) (sum/3);
      if (sum>127) //0
        gray_data[y*width+x] = 1;
      else
        gray_data[y*width+x] = 0;
    }
  }

  nPoints_ = contour_tracing (gray_data, width, height, sigma_, points_, input_file_name);

  //release the buffer
  delete gray_data;

  return nPoints_;
}

vcl_vector<vsol_point_2d_sptr>& ContourTracing::getResult (void)
{
  return points_;
}
