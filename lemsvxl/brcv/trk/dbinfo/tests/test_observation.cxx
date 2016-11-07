// This is brl/bseg/dbinfo/tests/test_tracking_face_2d.cxx
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
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
#include <vbl/io/vbl_io_smart_ptr.h>
static void test_observation(int argc, char* argv[])
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
  //Define a centered window
  vsol_point_2d_sptr p0 = new vsol_point_2d(10,10);
  vsol_point_2d_sptr p1 = new vsol_point_2d(20,10);
  vsol_point_2d_sptr p2 = new vsol_point_2d(20,20);
  vsol_point_2d_sptr p3 = new vsol_point_2d(10,20);
  vcl_vector<vsol_point_2d_sptr> verts;
  verts.push_back(p0);   verts.push_back(p1);  verts.push_back(p2); 
  verts.push_back(p3); 
  vsol_polygon_2d_sptr poly2 = new vsol_polygon_2d(verts);
  //Construct the observation
  /*dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, poly2);
  vcl_cout << "obs geometry " << *(obs->geometry()) << '\n';
  dbinfo_feature_base_sptr intf = new dbinfo_intensity_feature();
  dbinfo_feature_base_sptr gradf = new dbinfo_gradient_feature();
  vcl_vector<dbinfo_feature_base_sptr> features;
  features.push_back(intf);   features.push_back(gradf);
  obs->set_features(features);
  obs->scan(0, imgr);
  */
  dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, poly2, true, true, false);
  //  Test Binary I/O
  //Test writing the generic feature data pointer
  vsl_b_ofstream bp_out2("test_observation_io.tmp");
  vsl_b_write(bp_out2, obs);
  bp_out2.close();

  vsl_b_ifstream bp_in2("test_observation_io.tmp");
  dbinfo_observation_sptr obs_in;
  vsl_b_read(bp_in2, obs_in);
  bp_in2.close();
  if(obs_in)
    vcl_cout << "recovered observation " << *obs_in << '\n';
  vpl_unlink ("test_observation_io.tmp");
  if(!obs_in)
    {
      TEST("Binary read of observation pointer failed ", true, false);
      return;
    }
  TEST("Binary read of observation pointer ", obs_in->features().size(), 2);

}
TESTMAIN_ARGS(test_observation);
