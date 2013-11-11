//: 
// \file  test_bioproc_splr_filtering_proc.cxx
// \brief  compares with and without splr_symmetry proc
// \author    P. Klein
// \date        2006_06_27
// 
#include <testlib/testlib_test.h>
#include "bioproc_tests_modified_no_noise_filter_3d.h"
#include <xmvg/xmvg_composite_filter_3d.h>
#include <xscan/xscan_dummy_scan.h>
#include <proc/bioproc_splr_filtering_proc.h>
#include <proc/bioproc_filtering_proc.h>
#include <biob/biob_worldpt_roster_sptr.h>
#include <biob/biob_worldpt_roster.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>
#include <vcl_algorithm.h>

void test_bioproc_splr_proc_splats(){
  // scan
  xscan_dummy_scan scan(10, 40, 160, vnl_int_2(200, 200), vnl_double_2(100, 100),
      vgl_point_3d<double>(40.0, 0.0, 0.0), vnl_double_3(.0, .0, 1.), 
      2*vnl_math::pi/20, 5,  "/u/klein/pers//biotree/software/vxl/contrib/brl/blem/contrib/biotree/proc/tests/test_data/toy0###.tif");

  xscan_scan const & scan_ref = dynamic_cast<xscan_scan const&>(scan);

  // box
  double xmin, ymin, zmin, xmax, ymax, zmax;

  xmin = ymin = 0.0; zmin = 0.0;
  xmax = ymax = 0.1; zmax = 0.0;

  vgl_box_3d<double> box(xmin, ymin, zmin, xmax, ymax, zmax);

  // resolution
  double resolution = 0.05; // 50 micro

  // filter 3d
  double f_radius = 0.0125;
  double f_length = 0.1;
  vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);
  vgl_vector_3d<double> f_orientation_z(0.0, 0.0, 1.0);
  xmvg_no_noise_filter_descriptor fdz(f_radius, f_length, f_centre, f_orientation_z);
  // construct the filter
  bioproc_tests_modified_no_noise_filter_3d fz(fdz);

//  vcl_vector<xmvg_atomic_filter_3d<double> > filters;
  vcl_vector<bioproc_tests_modified_no_noise_filter_3d> filters;
  filters.push_back(fz);

  xmvg_composite_filter_3d<double, bioproc_tests_modified_no_noise_filter_3d> comp3d(filters);
  
  bioproc_splr_filtering_proc<double, bioproc_tests_modified_no_noise_filter_3d> splr_proc(scan, box, resolution, box, comp3d);
  biob_worldpt_roster_sptr ptr = const_cast <biob_worldpt_roster * > (splr_proc.sample_locations());
  bioproc_filtering_proc<double, bioproc_tests_modified_no_noise_filter_3d> proc(scan_ref, box, resolution, box, comp3d, ptr);

  unsigned int num_views = scan_ref.scan_size();
  unsigned int num_discrepancies = 0;
  for (unsigned int t = 0; t < num_views; ++t)
    {
    for (unsigned long int i = 0; i < splr_proc.sample_locations()->num_points(); ++i)
      {
       xmvg_composite_filter_2d<double> splr_splat = splr_proc.splr()->splat(t, biob_worldpt_index(i));
       xmvg_composite_filter_2d<double> dummy_splat = proc.splr()->splat(t, splr_proc.sample_locations()->point(biob_worldpt_index(i)));
       xmvg_atomic_filter_2d<double> splr_atomic_splat = splr_splat.atomic_filter(0);
       xmvg_atomic_filter_2d<double> dummy_atomic_splat = dummy_splat.atomic_filter(0);
       double splr_sum = splr_atomic_splat.weights_sum();
       double dummy_sum = dummy_atomic_splat.weights_sum();
       int discrepancy = vnl_math_abs(splr_sum - dummy_sum) > .1*vcl_max(vnl_math_abs(splr_sum), vnl_math_abs(dummy_sum));
       if (discrepancy){
                ++ num_discrepancies;
                vcl_cout << "******* DISCREPANCY ************\n";
                vcl_cout << "splr_splat:\n";
                vsl_print_summary(vcl_cout, splr_atomic_splat);
                {int c = 0;
                vnl_int_2 splr_splat_size = splr_atomic_splat.size();
                for (int j = 0; j < splr_splat_size[0]; ++j){
                  for (int k = 0; k < splr_splat_size[1]; ++k){
                   vcl_cout << splr_atomic_splat[j][k] << " ";
                  }
                  vcl_cout << "\n";
                }
                }
                vcl_cout << "dummy_splat:\n";
                vsl_print_summary(vcl_cout, dummy_atomic_splat);
                {int c = 0;
                for (int j = 0; j <  dummy_atomic_splat.size()[0]; ++j){
                  for (int k = 0; k <  dummy_atomic_splat.size()[1]; ++k){
                   vcl_cout << dummy_atomic_splat[j][k] << " ";
                  }
                  vcl_cout << "\n";
                }
                }
   
       }
       if (i % 1000 == 0 || discrepancy){
                vcl_cout << "orbit index " << t << " and point number " << i << "\n";
                vcl_cout << " splr weight: " << splr_sum
                         << " dummy weight: " << dummy_sum << "\n";
       }
      }
    }
  TEST("number of discrepancies", num_discrepancies, 0);
}

TESTMAIN(test_bioproc_splr_proc_splats);

