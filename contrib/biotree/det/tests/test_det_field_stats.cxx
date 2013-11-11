//: 
// \file     test_det_field_stats.cxx
// \brief    testing for field statistics class
// \author   H. C. Aras
// \date     2005-05-02
// 

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <det/det_field_stats.h>
#include <vcl_cstdlib.h>
#include <vcl_ctime.h>

static void test_det_field_stats()
{
  vcl_cout << "---test constructor---" << vcl_endl;
  // create a vector of responses randomly
  unsigned num_points = 100;
  unsigned num_filters = 2;
  vcl_vector< xmvg_filter_response<double> > responses(num_points);
  vcl_srand( (unsigned)vcl_time( NULL ) );
  for(unsigned i=0;i<num_points;i++)
  {
    vnl_vector<double> res(num_filters);
    
    for(unsigned k=0;k<num_filters;k++)
      res[k] = vcl_rand() / 1000.0;
    
    xmvg_filter_response<double> point_resp(res);
    responses[i] = point_resp;
  }
  det_field_stats fs(responses);
  TEST("number of points test", fs.responses_.size(), num_points);
  TEST("number of filters test", fs.responses_[0].size(), num_filters);

  det_field_stats fs2;
  // create xscan
  xscan_dummy_scan scan(10, 40, 160, vnl_int_2(200, 200), vnl_double_2(100, 100),
      vgl_point_3d<double>(40.0, 0.0, 0.0), vnl_double_3(.0, .0, 1.), 
      6*vnl_math::pi/180, 60,  "F:/MyDocs/Temp/toy_data_tif/toy0###.tif");
  // specify ROI by 3D box
  double xmin, ymin, zmin, xmax, ymax, zmax;
  xmin = ymin = zmin = -0.1;
  xmax = ymax = zmax = 0.1;
  vgl_box_3d<double> box(xmin, ymin, zmin, xmax, ymax, zmax);

  fs2.compute_responses_at(scan, box);
}

TESTMAIN(test_det_field_stats);
