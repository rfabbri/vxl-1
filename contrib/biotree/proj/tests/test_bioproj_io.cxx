//: 
// \file   test_bioproj_io.cxx
// \brief  test suite for bioproj_io class
// \author H. Can Aras
// \date   2006-07-12
// 
#include <testlib/testlib_test.h>
#include <proj/bioproj_io.h>

void test_bioproj_io()
{
  vcl_cout << "---testing constructor---" << vcl_endl;
  bioproj_io proj_io("G:\\newcast35um2\\newcast35um2_neg\\process\\newcastfragment35um2.scn", 
                     "G:\\newcast35um2\\newcast35um2_neg\\process\\newcast35um2_straighter_reg_box.bx3",
                     20.0, 46.8, 261.5, 345.712, vnl_int_2(590, 524), 0.9,
                     "G:\\newcast35um2\\newcast35um2_neg\\scan35um####.tif",
                     100.0, 100.0, 10, 5, 10);
  TEST("resolution", proj_io.resolution_, 20.0);
  TEST("sensor pixel size", proj_io.sensor_pix_size_, 46.8);
  TEST("source to origin distance", proj_io.source_origin_dist_, 261.5);
  TEST("source to sensor distance", proj_io.source_sensor_dist_, 345.712);
  TEST("sensor dimensions", proj_io.sensor_dim_, vnl_int_2(590, 524));
  TEST("number of views", proj_io.nviews_, 400);
  TEST("rotation step angle", proj_io.rot_step_angle_, 0.9);
  TEST("sigma r", proj_io.sigma_r_, 100.0);
  TEST("sigma z", proj_io.sigma_z_, 100.0);
  TEST("sigma r extent", proj_io.sigma_r_extent_, 10);
  TEST("sigma z extent", proj_io.sigma_z_extent_, 5);

  vcl_cout << proj_io.scan_ << vcl_endl;
  vcl_cout << proj_io.box_ << vcl_endl;
  vcl_cout << proj_io.file_path_ << vcl_endl;
}

TESTMAIN(test_bioproj_io);
