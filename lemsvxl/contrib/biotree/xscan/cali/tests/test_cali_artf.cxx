#include <xscan/xscan_dummy_scan.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_memory_image.h>
#include <vil/vil_new.h>
#include <cali/cali_simulated_scan_resource.h>
#include <cali/cali_artf_corresponder.h>
#include <cali/cali_calibrator.h>
#include <cali/cali_cylinder_artifact.h>
//#include <cali/cali_cylinder_create.h>
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <imgr/file_formats/imgr_skyscan_log_header.h>
#include <imgr/file_formats/imgr_skyscan_log.h>

#include <xscan/xscan_uniform_orbit.h>

static void test_cali_artf(int argc, char* argv[] )
{
 // static vcl_string fname = "C:\\Aug 8 05\\Bio-tree Aug 8 artifact.log";
     static vcl_string fname = "C:\\Artifact 2\\Artifact 2 .log";
  vcl_string conics_bin_file_base = "C:\\Aug 8 05\\bins\\Bio-tree Aug 8 artifact0000";
  //vcl_string conics_bin_file_base = "C:\\test_images\\working\\bins\\test_artifact0000";
 // cali_calibrator calib(fname, conics_bin_file_base, 819, 22);
cali_calibrator calib(fname, conics_bin_file_base, 819, 100);

  // create the artifact rotation, translation for the initial position
  vnl_vector<double> v(3);
  v[0] = 0;
  v[1] = 0;
  v[2] = 1;
  //double theta = (vnl_math::pi/2);//90 degrees
  vnl_quaternion<double> artf_rot(v, 1);
  // vnl_quaternion<double> artf_rot(v, 1.18);
  vgl_point_3d<double> artf_tr(0,0,0.1);
 // vgl_point_3d<double> artf_tr(0, 0, -14*cali_cylinder_artifact::height/16);

  /*
  double ball_indent_small = cali_cylinder_artifact::ball_indent_small;
  double ball_indent_big = cali_cylinder_artifact::ball_indent_big;
  double ball_spacing = cali_cylinder_artifact::ball_spacing;
  */

//  calib.calibrate(artf_rot, artf_tr, ball_indent_small, ball_indent_big, ball_spacing);
  //calib.calibrate(artf_rot, artf_tr);


  //xscan_dummy_scan scan(10,40.0,160.0,vnl_int_2(1024, 1024),vnl_double_2(512.0, 512.0));
  // create the scan from header file
  vcl_FILE *fp = vcl_fopen(fname.data(),"r");
  assert(fp != NULL);
  imgr_skyscan_log_header skyscan_log_header(fp);
  vcl_fclose(fp);
  
  imgr_skyscan_log skyscan_log(fname.data());
  xscan_scan scan;
  scan = skyscan_log.get_scan();
  vcl_cout << scan << vcl_endl;

  int nviews = scan.scan_size();
  vcl_cout << "scan SIZE---->" << nviews << vcl_endl;
  vcl_vector<vil_image_resource_sptr> resources(nviews);
  for (int i=0; i<nviews; i++) {
        resources[i] = vil_new_image_resource(2000, 1048,  1,  VIL_PIXEL_FORMAT_BYTE);
    }

  /*vpgl_calibration_matrix<double> kk(scan.kk());
  kk.set_focal_length(1);
  vgl_point_2d<double> pp = scan.kk().principal_point();
  vgl_point_2d<double> princp_point(pp.x()+200, pp.y()+200);
  //kk.set_principal_point(princp_point);
  scan.set_calibration_matrix(kk);*/
/*
  xscan_orbit_base_sptr orbit_base = (scan.orbit());
  xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
  vcl_cout << orbit << vcl_endl;
  vnl_quaternion<double> r0(orbit.r0().axis(), orbit.r0().angle() + 0.174);
  r0.normalize();
  xscan_orbit_base_sptr new_orbit = new xscan_uniform_orbit(orbit.rot(),
                        orbit.t(),
                        r0,
                        orbit.t0());
  scan.set_orbit(new_orbit);*/
/*
 cali_simulated_scan_resource scan_res = cali_simulated_scan_resource(scan, resources, 
    ball_indent_small, ball_indent_big, ball_spacing);
  */
cali_simulated_scan_resource scan_res = cali_simulated_scan_resource(scan, resources );
   

 // vcl_vector<vil_image_resource_sptr> images = scan_res.simulate_scan("C://test_images", "test_artifact", artf_rot, artf_tr, 22);
 vcl_vector<vil_image_resource_sptr> images = scan_res.simulate_scan("C://test_images", "test_artifact", artf_rot, artf_tr, 20);
  //cali_artf_corresponder corr;
  //corr.gen_corresp(images, "C:\\test_images\\real_artifact_scans\\Aug 8 05\\bins\\Bio-tree Aug 8 artifact", 400);
  
}

TESTMAIN_ARGS(test_cali_artf);
