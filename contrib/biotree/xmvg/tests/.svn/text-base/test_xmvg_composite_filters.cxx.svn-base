// This is /contrib/biotree/xmvg/tests/test_xmvg_composite_filters.cxx

//: 
// \file   test_xmvg_composite_filters.cxx
// \brief  testing suite for xmvg_composite_filter_2d and xmvg_composite_filter_3d
// \author H. Can Aras
// \date   2005-02-08
// 
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <xmvg/xmvg_no_noise_filter_descriptor.h>
#include <xmvg/xmvg_no_noise_filter_3d.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <vgl/vgl_point_3d.h>

static void test_xmvg_composite_filters()
{
  vcl_cout << "-----------------testing composite filter 3d------------------\n";
  vcl_cout << "---constructor---\n";
  // construct the filter descriptors
  double f_radius = 0.02;
  double f_length = 0.08;
  vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);
  vgl_vector_3d<double> f_orientation_z(0.0, 0.0, 1.0);
  vgl_vector_3d<double> f_orientation_y(0.0, 1.0, 0.0);
  vgl_vector_3d<double> f_orientation_x(1.0, 0.0, 0.0);
  xmvg_no_noise_filter_descriptor fdx(f_radius, f_length, f_centre, f_orientation_x);
  xmvg_no_noise_filter_descriptor fdy(f_radius, f_length, f_centre, f_orientation_y);
  xmvg_no_noise_filter_descriptor fdz(f_radius, f_length, f_centre, f_orientation_z);
  // construct the filters
  xmvg_no_noise_filter_3d fx(fdx);
  xmvg_no_noise_filter_3d fy(fdy);
  xmvg_no_noise_filter_3d fz(fdz);

//  vcl_vector<xmvg_atomic_filter_3d<double> > filters;
  vcl_vector<xmvg_no_noise_filter_3d> filters;
  filters.push_back(fx);
  filters.push_back(fy);
  filters.push_back(fz);

  xmvg_composite_filter_3d<double, xmvg_no_noise_filter_3d> comp3d(filters);
  TEST_NEAR("inner radius test", filters[0].descriptor().inner_radius(), 0.02, 1e-10);
  TEST_NEAR("length test", filters[1].descriptor().length(), 0.08, 1e-10);
  TEST("centre test", filters[2].descriptor().centre(), vgl_point_3d<double>(0.0,0.0,0.0));
  TEST("orientation test-1", filters[0].descriptor().orientation(), vgl_vector_3d<double>(1.0,0.0,0.0));
  TEST("orientation test-2", filters[1].descriptor().orientation(), vgl_vector_3d<double>(0.0,1.0,0.0));
  TEST("orientation test-3", filters[2].descriptor().orientation(), vgl_vector_3d<double>(0.0,0.0,1.0));

  vcl_cout << "---splat---" << vcl_endl;
  // construct the initial camera
  xmvg_source source;
  vnl_double_3x3 m(0.0);
  m[0][0]=13563.6;  m[0][1]=0;   m[0][2]=100;
  m[1][1]=13563.6;  m[1][2]=100;
  m[2][2]=1;
  vpgl_calibration_matrix<double> K(m);
  vnl_double_4x4 rot(0.0);
  rot[0][1]=1;
  rot[1][2]=-1;
  rot[2][0]=-1;
  rot[3][3]=1;
  vgl_h_matrix_3d<double> R(rot), Rinv = R.get_inverse();
  vgl_homg_point_3d<double> center(40.0,0.0,0.0);
  xmvg_perspective_camera<double> cam(K, center, R, source);
  // call composite filter splat
  xmvg_composite_filter_2d<double> comp2d(comp3d.splat(cam, vgl_point_3d<double>(0.0, 0.0, 0.0)));
  // call atomic filter splats one by one to test the validity of composite filter
  xmvg_atomic_filter_2d<double> resx(fx.splat(vgl_point_3d<double> (0.0, 0.0, 0.0), cam));
  xmvg_atomic_filter_2d<double> resy(fy.splat(vgl_point_3d<double> (0.0, 0.0, 0.0), cam));
  xmvg_atomic_filter_2d<double> resz(fz.splat(vgl_point_3d<double> (0.0, 0.0, 0.0), cam));
  TEST("filter-x size test", comp2d.atomic_filter(0).size(), resx.size());
  TEST("filter-y size test", comp2d.atomic_filter(1).size(), resy.size());
  TEST("filter-z size test", comp2d.atomic_filter(2).size(), resz.size());
  TEST("filter-x location test", comp2d.atomic_filter(0).location(), resx.location());
  TEST("filter-y location test", comp2d.atomic_filter(1).location(), resy.location());
  TEST("filter-z location test", comp2d.atomic_filter(2).location(), resz.location());
  TEST_NEAR("filter-x weights sum test", comp2d.atomic_filter(0).weights_sum(), resx.weights_sum(), 1e-09);
  TEST_NEAR("filter-y weights sum test", comp2d.atomic_filter(1).weights_sum(), resy.weights_sum(), 1e-09);
  TEST_NEAR("filter-z weights sum test", comp2d.atomic_filter(2).weights_sum(), resz.weights_sum(), 1e-09);

  int i,j;
  double sum = 0;
  comp2d.eliminate_residue_effect();
  int sx = comp2d.atomic_filter(0).size().get(0);
  int sy = comp2d.atomic_filter(0).size().get(1);
  for(i=0; i<sx; i++)
    for(j=0; j<sy; j++)
      sum += comp2d.atomic_filter(0).weights().get(i,j);
  vcl_cout << "Weights sum before eliminating residue: " << comp2d.atomic_filter(0).weights_sum() << vcl_endl;
  vcl_cout << "Weights sum after eliminating residue: " << sum << vcl_endl;
  TEST_NEAR("eliminating splat residue test for filter 0", sum, 0, 1e-09);

  sum = 0;
  sx = comp2d.atomic_filter(1).size().get(0);
  sy = comp2d.atomic_filter(1).size().get(1);
  for(i=0; i<sx; i++)
    for(j=0; j<sy; j++)
      sum += comp2d.atomic_filter(1).weights().get(i,j);
  vcl_cout << "Weights sum before eliminating residue: " << comp2d.atomic_filter(1).weights_sum() << vcl_endl;
  vcl_cout << "Weights sum after eliminating residue: " << sum << vcl_endl;
  TEST_NEAR("eliminating splat residue test for filter 0", sum, 0, 1e-09);

  sum = 0;
  sx = comp2d.atomic_filter(2).size().get(0);
  sy = comp2d.atomic_filter(2).size().get(1);
  for(i=0; i<sx; i++)
    for(j=0; j<sy; j++)
      sum += comp2d.atomic_filter(2).weights().get(i,j);
  vcl_cout << "Weights sum before eliminating residue: " << comp2d.atomic_filter(2).weights_sum() << vcl_endl;
  vcl_cout << "Weights sum after eliminating residue: " << sum << vcl_endl;
  TEST_NEAR("eliminating splat residue test for filter 0", sum, 0, 1e-09);
}

TESTMAIN(test_xmvg_composite_filters);
