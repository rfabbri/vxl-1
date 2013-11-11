#include <testlib/testlib_test.h>

#include <dbgl/dbgl_poly_curve_line.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbcvr/dbcvr_interp_cvmatch.h>

#include <vcl_iostream.h>
#include <vcl_cmath.h>

#define NearZeroValue    (1.0e-8)

MAIN( test_interp_cvmatch )
{
  START ("Interpolated Curve 2d Matching Test");

  vcl_cout << "construct first line:" << vcl_endl;
  vgl_point_2d<double> p0(-1.0, -1.0);  // 0
  vgl_point_2d<double> p1(0.0, 0.0);   
  vgl_point_2d<double> p2(1.0, 0.0);
  
  vcl_vector<dbgl_param_curve *> ints(2);
  ints[0] = new dbgl_poly_curve_line(p0, p1);
  ints[1] = new dbgl_poly_curve_line(p1, p2);

  dbsol_interp_curve_2d_sptr c1 = new dbsol_interp_curve_2d(ints);
  
  vcl_cout << "construct second line:" << vcl_endl;
  vgl_point_2d<double> p3(-1.0, 0.0);  // 0
  vgl_point_2d<double> p4(0.0, 1.0);   
  vgl_point_2d<double> p5(1.0, 1.0);
  
  vcl_vector<dbgl_param_curve *> ints2(2);
  ints2[0] = new dbgl_poly_curve_line(p3, p4);
  ints2[1] = new dbgl_poly_curve_line(p4, p5);

  dbsol_interp_curve_2d_sptr c2 = new dbsol_interp_curve_2d(ints2);

  double L = c1->length();
  double L_bar = c2->length();

  dbcvr_interp_cvmatch cvm(c1, c2, 10.0f, (L+L_bar)/6, (L+L_bar)/6);
  cvm.Match();
  TEST("match() ", vcl_fabs(cvm.finalCost()-0.0)<NearZeroValue, true);

  dbcvr_interp_cvmatch cvm2(c1, c2, 10.0f, (L+L_bar)/12, (L+L_bar)/12);
  cvm2.Match();
  TEST("match() 2", vcl_fabs(cvm2.finalCost()-0.0)<NearZeroValue, true);
  
  SUMMARY();
}
