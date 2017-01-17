// This is brl/bseg/dbinfo/tests/test_observation_matcher.cxx
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vbl/vbl_array_1d.h>
#include <vul/vul_timer.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_new.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_rectangle_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <brip/brip_vil_float_ops.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_intensity_feature.h>
#include <dbinfo/dbinfo_gradient_feature.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_matcher.h>
static void test_observation_matcher(int argc, char* argv[])
{
  //
  //====================  OBSERVATION  ================================
  //
  //Test image for observation
  unsigned cols = 31,  rows = 31;
  vil_image_view<unsigned char> gimage(cols, rows, 1);
  for(unsigned r = 0; r<rows; ++r)
    for(unsigned c = 0; c<cols; ++c)
      gimage(c,r) = r + c;
  vil_image_resource_sptr imgr = vil_new_image_resource_of_view(gimage);
  //define a 5x5 window
  vsol_rectangle_2d* rect = new vsol_rectangle_2d(new vsol_point_2d(15, 15),
                                                  5, 5, 0);
  vsol_polygon_2d_sptr poly = rect;

  dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, poly);
  dbinfo_feature_base_sptr intf = new dbinfo_intensity_feature();
  dbinfo_feature_base_sptr gradf = new dbinfo_gradient_feature();
  vcl_vector<dbinfo_feature_base_sptr> features;
  features.push_back(intf);   features.push_back(gradf);
  obs->set_features(features);
  obs->scan(0, imgr);
  vcl_cout << "Total Mutual Information " 
           << dbinfo_observation_matcher::minfo(obs, obs) << '\n';

  TEST_NEAR("Test total observation mutual information ",
            dbinfo_observation_matcher::minfo(obs, obs), 1.51565, 1e-5);
#if 1
  //Test speed
  vul_timer t;
  int i = 0;
  for(; i<1000; ++i)
    {
      dbinfo_observation_sptr temp = 
        new dbinfo_observation(0, imgr, poly, features);
      temp->scan(0, imgr);
      //temp->minfo();
    }
  vcl_cout << "Construct and evaluate ("
           << i  << ") observations in "
           << t.real() << " msecs.\n"<< vcl_flush;
#endif
}
TESTMAIN_ARGS(test_observation_matcher);
