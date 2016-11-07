// This is brl/bseg/dbinfo/tests/test_tracking_face_2d.cxx
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
#include <dbinfo/dbinfo_observation_generator.h>
static void test_observation_generator(int argc, char* argv[])
{
  //
  //====================  OBSERVATION  GENERATOR ============================
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
  dbinfo_region_geometry_sptr g = obs->geometry();
  vcl_cout << "Initial Point " << g->point(0) << '\n';
  //===============  generate random observations =========
  unsigned n = 10;
  vcl_vector<dbinfo_observation_sptr> observs;
  dbinfo_observation_generator::uniform_about_seed(n, obs, observs, 17.0, 0.0);
  for(unsigned i = 0; i<observs.size(); ++i)
    {
      dbinfo_observation_sptr ob = observs[i];
      dbinfo_region_geometry_sptr tg = ob->geometry();
      vcl_cout << "T(p)[" << i << "]= " << tg->point(0) << '\n';
    }
  TEST("Test observation generator ", observs.size(), 10);
}
TESTMAIN_ARGS(test_observation_generator);
