//: 
// \file  test_bioproc_splr_filtering_proc.cxx
// \brief  compares with and without splr_symmetry proc
// \author    P. Klein
// \date        2006_06_27
// 
#include <testlib/testlib_test.h>
#include <xmvg/xmvg_no_noise_filter_3d.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <xscan/xscan_dummy_scan.h>
#include <proc/bioproc_splr_filtering_proc.h>
#include <proc/bioproc_filtering_proc.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>

void test_bioproc_splr_proc(){
  // scan
  xscan_dummy_scan scan(10, 40, 160, vnl_int_2(200, 200), vnl_double_2(100, 100),
      vgl_point_3d<double>(40.0, 0.0, 0.0), vnl_double_3(.0, .0, 1.), 
                   2*vnl_math::pi/180, 5, "/ltmp/image_data/aug8_straight_and_angled_hairs/negatives/Straight hairs0###.tif");

  xscan_scan const & scan_ref = dynamic_cast<xscan_scan const&>(scan);

  // box
  double xmin, ymin, zmin, xmax, ymax, zmax;

  xmin = 0.68469;
  ymin = -1.56143;
  zmin = zmax = 0.;
  xmax = 1.48469;
  ymin = -0.76143;
    //  xmin = ymin =  -0.2;
    //  xmax = ymax = zmin = zmax = 0.2;

  vgl_box_3d<double> box(xmin, ymin, zmin, xmax, ymax, zmax);

  // resolution
  double resolution = 0.005; // 50 micro
  //  double resolution = 0.025; // 50 micro

  // filter 3d
  double f_radius = 0.0125;
  double f_length = 0.1;
  vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);
  vgl_vector_3d<double> f_orientation_z(0.0, 0.0, 1.0);
  xmvg_no_noise_filter_descriptor fdz(f_radius, f_length, f_centre, f_orientation_z);
  // construct the filters
  xmvg_no_noise_filter_3d fz(fdz);

//  vcl_vector<xmvg_atomic_filter_3d<double> > filters;
  vcl_vector<xmvg_no_noise_filter_3d> filters;
  filters.push_back(fz);

  xmvg_composite_filter_3d<double, xmvg_no_noise_filter_3d> comp3d(filters);

  // inner box
  vgl_vector_3d<double> scale(0.3, 0.3, 0.3);
  vgl_point_3d<double> centroid = box.centroid();
  double centroid_array[3] = {centroid.x(), centroid.y(), centroid.z()};
  vgl_box_3d<double> smallbox(centroid_array, scale.x()*box.width(), scale.y()*box.height(),  scale.z()*box.depth(), vgl_box_3d<double>::centre);
 
  bioproc_splr_filtering_proc<double, xmvg_no_noise_filter_3d> splr_proc(scan, smallbox, resolution, box, comp3d);
  biob_worldpt_roster_sptr ptr = splr_proc.response_field().roster();
  bioproc_filtering_proc<double, xmvg_no_noise_filter_3d> proc(scan_ref, smallbox, resolution, box, comp3d,  ptr);

  splr_proc.execute();
  proc.execute();

  biob_worldpt_field<xmvg_filter_response<double> > & responses = proc.worldpt_field();
  vcl_vector<xmvg_filter_response<double> > & samples = responses.values();

  vcl_vector<xmvg_filter_response<double> > const & splr_samples = splr_proc.response_field().const_values();
//  vcl_vector<xmvg_filter_response<double> > const & samples = proc.responses();
  double max_rel_error = 0.0;
  double sum_rel_errors = 0.0;
  TEST("splr_proc sample size equals proc sample size", splr_samples.size(), samples.size());
  unsigned long int count = 0;
  for (unsigned long int i = 0; i < splr_samples.size(); ++i){
    double magnitude = vcl_max(vnl_math_abs(samples[i][0]),vnl_math_abs(splr_samples[i][0])); 
    if (magnitude > 100000){
      double rel_error = vnl_math_abs((splr_samples[i][0] - samples[i][0])/magnitude);
      //    max_rel_error = vcl_max(max_rel_error, rel_error);
      if (rel_error > max_rel_error){
      max_rel_error = rel_error;
      vcl_cout << "test_bioproc_splr_proc max rel error: " << max_rel_error << "\n";
      }
      sum_rel_errors += rel_error;
      count ++;
    }
  }
  double avg_rel_error = sum_rel_errors/splr_samples.size();
  vcl_cout <<  "test_bioproc_splr_proc avg rel error: " << avg_rel_error << "\n";
  TEST("small maximum relative error", max_rel_error < .1, true);
  TEST("small average relative error", avg_rel_error < .01, true);
}

TESTMAIN(test_bioproc_splr_proc);

