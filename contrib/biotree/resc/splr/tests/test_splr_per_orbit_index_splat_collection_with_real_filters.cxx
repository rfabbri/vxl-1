#include <testlib/testlib_test.h>
#include <biob/biob_grid_worldpt_roster.h>
#include <splr/splr_per_orbit_index_splat_collection.txx>
#include <xmvg/xmvg_no_noise_filter_descriptor.h>
#include <xmvg/xmvg_no_noise_filter_3d.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <xscan/xscan_dummy_scan.h>

//template class splr_per_orbit_index_splat_collection<double, vcl_vector<

void test_splr_per_orbit_index_splat_collection_with_real_filters(){
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
  // construct the initial camera
  xmvg_source source;
  vnl_double_3x3 m(0.0);
  m[0][0]=13563.6;  m[0][1]=0;   m[0][2]=100;
  m[1][1]=13563.6;  m[1][2]=100;
  m[2][2]=1;
  dvpgl_calibration_matrix<double> K(m);
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

  // make a roster to hold one point, the origin
   biob_worldpt_box box(worldpt(0,0,0), worldpt(.1, .1, .1));
   biob_grid_worldpt_roster roster(box, 1);

   //make an xscan_dummy_scan
   xscan_dummy_scan scan;
   
   //make the splat collection
   splr_per_orbit_index_splat_collection<double,  xmvg_no_noise_filter_3d> splat_collection(scan, &comp3d, &roster);

   //populate the splat collection at t = 0;
  splat_collection.populate(0, biob_worldpt_index(0));

  // write to a file
  vsl_b_ofstream  outfs("test_file");
  vsl_b_write(outfs, splat_collection);
  outfs.close();
  // read from the file
  splr_per_orbit_index_splat_collection<double,  xmvg_no_noise_filter_3d> splat_collection2(scan, &comp3d, &roster);
  std::cout << "test_splr_splat_collection about to read from file\n";

  vsl_b_ifstream infs("test_file");
  vsl_b_read(infs, splat_collection2);


   //get the camera from the scan
   xmvg_perspective_camera<double> scancamera = scan(0);

    //see if splat obtained from camera is  the same as that obtained from the splat_collection
    TEST("same filter retrieved", splat_collection.splat(0, biob_worldpt_index(0)) == comp3d.splat(scancamera, roster.point(biob_worldpt_index(0))), true);

    TEST("splat from original splat collection == splat from splat collection from file",
      splat_collection.splat(0, biob_worldpt_index(0)) ==  splat_collection2.splat(0, biob_worldpt_index(0)), true);

}
TESTMAIN(test_splr_per_orbit_index_splat_collection_with_real_filters);
