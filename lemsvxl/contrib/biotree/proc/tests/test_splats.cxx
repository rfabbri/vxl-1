//: 
// \file  test_splats.cxx
// \brief  compares with and without splr_symmetry proc
// \author    P. Klein
// \date        2006_07_12
// 
#include <testlib/testlib_test.h>
#include <xmvg/xmvg_no_noise_filter_3d.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <xscan/xscan_dummy_scan.h>
#include <proc/bioproc_splr_filtering_proc.h>
#include <proc/bioproc_filtering_proc.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>
#include <vcl_cmath.h>

void test_splats(){
  // scan
  xscan_dummy_scan scan(10, 40, 160, vnl_int_2(200, 200), vnl_double_2(100, 100),
      vgl_point_3d<double>(40.0, 0.0, 0.0), vnl_double_3(.0, .0, 1.), 
      2*vnl_math::pi/180, 5, "");

  // filter 3d
  double f_radius = 0.0125;
  double f_length = 0.1;
  vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);
  vgl_vector_3d<double> f_orientation_z(0.0, 0.0, 1.0);
  xmvg_no_noise_filter_descriptor fdz(f_radius, f_length, f_centre, f_orientation_z);
  xmvg_no_noise_filter_3d fz(fdz);

  worldpt rep_pt(0.0, .05, -.05);

  double rot_angle = 2*vnl_math::pi/180;

  vnl_quaternion<double> camera0_to_camera1 = dynamic_cast<xscan_uniform_orbit*>(scan.orbit().ptr())->rot().inverse();
  vnl_double_3x3 R(camera0_to_camera1.rotation_matrix_transpose());
  vgl_h_matrix_3d<double> vgmat(R.transpose(),vnl_double_3(0.,0.,0.));
  biob_worldpt_transformation transformation(vgmat);
  worldpt transformed_pt = rep_pt;
  for (int i = 1; i <= 9; ++i){
      xmvg_atomic_filter_2d<double> rep_splat = fz.splat(rep_pt, scan(i));      
      //xmvg_composite_filter_2d<double> rep_splat = proc.splr()->splat(i, rep_pt);
      transformed_pt = transformation(transformed_pt);
      vcl_cout << "transformed point: " << transformed_pt << "\n";
      vcl_cout << .05*sin(i*rot_angle) << " " << .05*cos(i*rot_angle) << " " << -.05 << "\n";
      xmvg_atomic_filter_2d<double> transformed_pt_splat = fz.splat(transformed_pt, scan(0));      
    //  xmvg_composite_filter_2d<double> transformed_pt_splat = proc.splr()->splat(0, transformed_pt);
      vcl_cout << "rotation: " << i << " rep_splat size: " << rep_splat.size() << " transformed_pt_splat size: " << transformed_pt_splat.size() << "\n";
  }
}

TESTMAIN(test_splats);

