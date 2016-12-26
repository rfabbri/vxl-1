//---------------------------------------------------------------------
// This is contrib/ozge/curvefitting_command.cxx
//:
// \file
// \brief command line program that takes <contour_name1>.con <min_length> <rms_distance> 
//        line segments are fitted via vgl and output curve is written.
//
// \author
//  O.C. Ozcanli - September 28, 2004
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vgl/algo/vgl_fit_lines_2d.h>
#include <bsol/bsol_intrinsic_curve_2d_sptr.h>
#include <bsol/bsol_intrinsic_curve_2d.h>

/* executable to generate fitted curve given a contour */
/* if there is a gap between two consecutive fitted line segments,
    just connects their endpoints 
  set min_fit_length to 2, not to leave any gaps!!! */

int main( int argc, char* argv[] )
{
  int i;
  char strtemp1[100];
  char strtemp2[100];

  if (argc < 3) {
    vcl_cout << "USAGE: curvefitting_command.exe <contour_name1>.con <rms_distance>\n";
    exit(-1);
  }

  vcl_string con_filename(argv[1]);

  bsol_intrinsic_curve_2d_sptr curve_2d = new bsol_intrinsic_curve_2d;
  curve_2d->readCONFromFile( con_filename.c_str() );
  
  int min_fit_length = 2;
  float rms_distance;
  rms_distance = atof(argv[2]);

  vcl_cout << "Number of points in original curve: " << curve_2d->size() << vcl_endl;

  vgl_fit_lines_2d<double> fitter;//the fitting class
  fitter.set_min_fit_length(min_fit_length);
  fitter.set_rms_error_tol((double)rms_distance);

  for (int i = 0; i<curve_2d->size(); i++) {
    vgl_point_2d<double> p(curve_2d->x(i), curve_2d->y(i));
    fitter.add_point(p);
  }
  curve_2d->clear();

  fitter.fit();
  vcl_vector<vgl_line_segment_2d<double> >& segs = fitter.get_line_segs();
  vcl_cout << "Number of line segments in fitted curve: " << segs.size() << vcl_endl;

  bsol_intrinsic_curve_2d_sptr curve_2d_fitted = new bsol_intrinsic_curve_2d();

  curve_2d_fitted->add_vertex(segs[0].point1().x(),segs[0].point1().y());
  curve_2d_fitted->add_vertex(segs[0].point2().x(),segs[0].point2().y());
  for (int i = 1; i<static_cast<int>(segs.size()); i++) {
    curve_2d_fitted->add_vertex(segs[i].point2().x(),segs[i].point2().y());
  }
  vcl_cout << "Number of points in fitted curve: " << curve_2d_fitted->size() << vcl_endl;

  strcpy(strtemp1, con_filename.c_str());    
  for ( i=0; strtemp1[i] != '.'; i++ ){}
  strtemp1[i]= 0;
  sprintf(strtemp2, "%s-%d-%.4f", strtemp1, min_fit_length, rms_distance);
  sprintf(strtemp2, "%s.con", strtemp2);
  
  FILE *out = fopen(strtemp2, "w");
  fprintf(out, "CONTOUR\nOPEN\n%d\n", curve_2d_fitted->size());
  for (int i = 0; i<curve_2d_fitted->size(); i++)
    fprintf(out, "%lf %lf\n", curve_2d_fitted->x(i), curve_2d_fitted->y(i));
  
  fclose(out);
  return 0;
}

/* executable to generate fitted curve given a contour */
/* if there is a gap between two consecutive fitted line segments,
    just connects their endpoints 
  set min_fit_length to 2, not to leave any gaps!!! */

int connect_gaps( int argc, char* argv[] )
{
  int i;
  char strtemp1[100];
  char strtemp2[100];

  if (argc < 4) {
    vcl_cout << "USAGE: curvematching_command.exe <contour_name1>.con <min_length> <rms_distance>\n";
    exit(-1);
  }



  vcl_string con_filename(argv[1]);

  bsol_intrinsic_curve_2d_sptr curve_2d = new bsol_intrinsic_curve_2d;
  curve_2d->readCONFromFile( con_filename.c_str() );
  
  int min_fit_length;
  float min_fit_length_sqr;
  min_fit_length = atoi(argv[2]);
  min_fit_length_sqr = (float)(min_fit_length*min_fit_length)/16.0f;
  float rms_distance;
  rms_distance = atof(argv[3]);

  vcl_cout << "Number of points in original curve: " << curve_2d->size() << vcl_endl;

  vgl_fit_lines_2d<double> fitter;//the fitting class
  fitter.set_min_fit_length(min_fit_length);
  fitter.set_rms_error_tol((double)rms_distance);

  for (int i = 0; i<curve_2d->size(); i++) {
    vgl_point_2d<double> p(curve_2d->x(i), curve_2d->y(i));
    fitter.add_point(p);
  }
  curve_2d->clear();

  fitter.fit();
  vcl_vector<vgl_line_segment_2d<double> >& segs = fitter.get_line_segs();
  vcl_cout << "Number of line segments in fitted curve: " << segs.size() << vcl_endl;

  bsol_intrinsic_curve_2d_sptr curve_2d_fitted = new bsol_intrinsic_curve_2d();

  curve_2d_fitted->add_vertex(segs[0].point1().x(),segs[0].point1().y());
  curve_2d_fitted->add_vertex(segs[0].point2().x(),segs[0].point2().y());
  for (int i = 1; i<static_cast<int>(segs.size()); i++) {
    if ((segs[i-1].point2()-segs[i].point1()).sqr_length() > min_fit_length_sqr)
      curve_2d_fitted->add_vertex(segs[i].point1().x(),segs[i].point1().y());
    curve_2d_fitted->add_vertex(segs[i].point2().x(),segs[i].point2().y());
  }
  vcl_cout << "Number of points in fitted curve: " << curve_2d_fitted->size() << vcl_endl;

  strcpy(strtemp1, con_filename.c_str());    
  for ( i=0; strtemp1[i] != '.'; i++ ){}
  strtemp1[i]= 0;
  sprintf(strtemp2, "%s-%d-%.4f", strtemp1, min_fit_length, rms_distance);
  sprintf(strtemp2, "%s.con", strtemp2);
  vcl_cout << "output file name: " << strtemp2 << vcl_endl;
  
  FILE *out = fopen(strtemp2, "w");
  fprintf(out, "CONTOUR\nOPEN\n%d\n", curve_2d_fitted->size());
  for (int i = 0; i<curve_2d_fitted->size(); i++)
    fprintf(out, "%lf %lf\n", curve_2d_fitted->x(i), curve_2d_fitted->y(i));
  
  fclose(out);
  return 0;
}
