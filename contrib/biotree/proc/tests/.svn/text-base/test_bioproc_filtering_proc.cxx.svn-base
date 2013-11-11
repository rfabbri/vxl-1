//: 
// \file  test_bioproc_filtering_proc.h
// \brief  testing the bioproc_filtering_proc class
// \author    Kongbin Kang
// \date        2005-03-03
// 
//
// 2005-07-24
// changed it to use xmvg_point_filter for testing


#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <xmvg/xmvg_point_filter_3d.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <xscan/xscan_dummy_scan.h>
#include <proc/bioproc_filtering_proc.h>

static void test_bioproc_filtering_proc(int argc, char* argv[] )
{

  // scan
  //
  double src_rot_dist = 40;
  double sensor_src_dist = 160;
  int u0 = 100, v0 = 100;
  int sx = 200, sy = 200;
  double pixel_size = 10;
  
  xscan_dummy_scan scan(
      pixel_size, 
      src_rot_dist, 
      sensor_src_dist, 
      vnl_int_2(sx, sy), 
      vnl_double_2(u0, v0),
      vgl_point_3d<double>(0.0, -src_rot_dist, 0.0), 
      vnl_double_3(.0, .0, 1.), 
      2*vnl_math::pi/10, 10,  
      argv[1] ); // argv[1] is the path and file pattern of image files

  xscan_scan const & scan_ref = dynamic_cast<xscan_scan const&>(scan);

  // box
  double xmin, ymin, zmin, xmax, ymax, zmax;

  xmin = ymin = zmin = -0.1;
  xmax = ymax = zmax = 0.1;

  double resolution = 0.05;
  
  vgl_box_3d<double> box(xmin, ymin, zmin, xmax, ymax, zmax);

  //unused variable double f_length = 0.1;
  vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);
  xmvg_point_filter_descriptor fd;
  // construct the filters
  xmvg_point_filter_3d f(fd);

  vcl_vector<xmvg_point_filter_3d> filters;
  filters.push_back(f);

  xmvg_composite_filter_3d<double, xmvg_point_filter_3d> comp3d(filters);
 
  // inner box
  vgl_vector_3d<double> scale(0.5, 0.5, 0.5);
  vgl_point_3d<double> centroid = box.centroid();
  double centroid_array[3] = {centroid.x(), centroid.y(), centroid.z()};
  vgl_box_3d<double> smallbox(centroid_array, scale.x()*box.width(), scale.y()*box.height(),  scale.z()*box.depth(), vgl_box_3d<double>::centre);

  bioproc_filtering_proc<double, xmvg_point_filter_3d> proc(scan_ref, box, resolution, smallbox, comp3d);

  // filtering
  proc.execute();

  biob_worldpt_field<xmvg_filter_response<double> > & responses = proc.worldpt_field();
  vcl_vector<xmvg_filter_response<double> > & field = responses.values();

  // check response value
  biob_grid_worldpt_roster bgwr = static_cast<const biob_grid_worldpt_roster&>(*(responses.roster()));


  unsigned long num_points = bgwr.num_points();

  for(int i=0; i<static_cast<int>(num_points); i++){

    vgl_point_3d<double> pt3d = bgwr.point(i);

    double R = vcl_sqrt(pt3d.x()*pt3d.x() + pt3d.y()*pt3d.y());

    double a = src_rot_dist;
    double b = sensor_src_dist;
    double l = 25*pixel_size / 1000;

    // forground intensity
    double I0 = 20000;


    double value = 0;

    if(R > a*l/vcl_sqrt(l*l + b*b))
      value = I0*(int)vcl_ceil(10*(1-2*vcl_acos(a*l/vcl_sqrt(l*l + b*b)/R)/vnl_math::pi)) ;
    else
      value = I0*10;

    vcl_cout << pt3d << "\n";
    TEST_NEAR("testing reponse ", field[i][0], value, 1e-6);
  }
}

TESTMAIN_ARGS(test_bioproc_filtering_proc)

