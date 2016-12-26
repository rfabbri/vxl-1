// This is brl/bseg/dbinfo/tests/test_osl.cxx
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_array_1d.h>
#include <vul/vul_timer.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/io/vbl_io_smart_ptr.h>
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
#include <dbinfo/dbinfo_osl.h>
static void test_osl(int argc, char* argv[])
{
  //
  //====================  OSL  ================================
  //
  //Test image for osl
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
  dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, poly2);
  vcl_cout << "obs geometry " << *(obs->geometry()) << '\n';
  dbinfo_feature_base_sptr intf = new dbinfo_intensity_feature();
  dbinfo_feature_base_sptr gradf = new dbinfo_gradient_feature();
  vcl_vector<dbinfo_feature_base_sptr> features;
  features.push_back(intf);   features.push_back(gradf);
  obs->set_features(features);
  obs->scan(0, imgr);
  //set the doc string
  obs->set_doc("An observation");
  // create an osl
  vcl_vector<vcl_string> classes;

  classes.push_back("class1");   classes.push_back("class2");
  vcl_vector<vcl_vector<dbinfo_observation_sptr> > prototypes(2);
  vcl_vector<dbinfo_observation_sptr> protos;
  protos.push_back(obs);
  prototypes[0]=protos;
  protos.push_back(obs);
  prototypes[1]=protos;
  dbinfo_osl_sptr osl = new dbinfo_osl(classes, prototypes);
  vcl_cout << *osl << '\n';
  //  Test Binary I/O
  //Test writing the generic feature data pointer
  vsl_b_ofstream bp_out2("test_observation_io.tmp");
  vsl_b_write(bp_out2, osl);
  bp_out2.close();

  vsl_b_ifstream bp_in2("test_observation_io.tmp");
  dbinfo_osl_sptr osl_in = 0;
  vsl_b_read(bp_in2, osl_in);
  bp_in2.close();
  if(osl_in)
    vcl_cout << "recovered observation " << *osl_in << '\n';
  vpl_unlink ("test_observation_io.tmp");
  if(!osl_in)
    {
      TEST("Binary read of osl  pointer failed ", true, false);
      return;
    }
  vcl_vector<vcl_string> in_classes = osl_in->classes();
  bool good = in_classes.size() == 2;
  if(good)
    good = in_classes[0]=="class1"&&in_classes[1]=="class2";
  TEST("Binary read of osl pointer ", good , true );
  //test remove prototype
  osl_in->remove_prototype("class2", "An observation");
  vcl_vector<dbinfo_observation_sptr> protos2;
  good = osl_in->prototypes("class2", protos2);
  if(good)
    {
      good = good && protos2.size()==1;
    }
  TEST("Remove prototype ", good , true );
  //test remove class
  good = osl_in->remove_class("class2");
  if(good)
    {
      good = good && !osl_in->prototypes("class2", protos2);
    }
  TEST("Remove class ", good , true );
}
TESTMAIN_ARGS(test_osl);
