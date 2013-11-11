// This is brl/brcv/rec/dbru/tests/test_osl.cxx
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_array_1d.h>
#include <vul/vul_timer.h>
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
//#include <dbinfo/dbinfo_feature_format.h>
//#include <dbinfo/dbinfo_feature_data.h>
//#include <dbinfo/dbinfo_intensity_feature.h>
//#include <dbinfo/dbinfo_gradient_feature.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbru/dbru_osl.h>
#include <dbru/dbru_object.h>
#include <dbru/dbru_label.h>

static void test_osl(int argc, char* argv[])
{
  //
  //====================  OSL  ================================
  //
  //Test image for osl
  unsigned cols = 50,  rows = 50;
  vil_image_view<unsigned char> gimage(cols, rows, 1);
  for(unsigned r = 0; r<rows; ++r)
    for(unsigned c = 0; c<cols; ++c)
      gimage(c,r) = r + c;
  vil_image_resource_sptr imgr = vil_new_image_resource_of_view(gimage);
  vcl_cout << "created the image\n";
  //Define a centered window
  vsol_point_2d_sptr p0 = new vsol_point_2d(10,10);
  vsol_point_2d_sptr p1 = new vsol_point_2d(20,10);
  vsol_point_2d_sptr p2 = new vsol_point_2d(20,20);
  vsol_point_2d_sptr p3 = new vsol_point_2d(10,20);
  vcl_vector<vsol_point_2d_sptr> verts;
  verts.push_back(p0);   verts.push_back(p1);  verts.push_back(p2); 
  verts.push_back(p3); 
  vsol_polygon_2d_sptr poly2 = new vsol_polygon_2d(verts);
  vcl_cout << "created the polygon\n";
  vcl_vector<vsol_point_2d_sptr> verts2;
  verts2.push_back(p1); verts2.push_back(p3); verts2.push_back(p0);
  vsol_polygon_2d_sptr poly3 = new vsol_polygon_2d(verts2);

  dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, poly2, true, true, false);
  dbinfo_observation_sptr obs2 = new dbinfo_observation(0, imgr, poly3, true, true, false);
  vcl_cout << "created the observation\n";
 //set the doc string
  obs->set_doc("An observation");
  obs2->set_doc("An observation222");
  // create an osl
  vcl_vector<vcl_string> classes;

  // test add_object
  dbru_object_sptr obj = new dbru_object(1000,0,0);
  obj->add_observation(obs);
  obj->add_polygon(poly2,new dbru_label("class1", 0, 0, 0, 0));

  dbru_osl_sptr osl1 = new dbru_osl();

  osl1->add_object(obj);
  TEST("dbru_osl::add_object() ", osl1->n_objects(), 1);
  
  dbru_object_sptr obj2 = new dbru_object(1000,2,3);
  obj2->add_observation(obs);
  obj2->add_polygon(poly2,new dbru_label("class2", 0, 0, 0, 0));
  
  obj2->add_observation(obs2);
  obj2->add_polygon(poly3, new dbru_label("class2", 0, 0, 0, 0));

  osl1->add_object(obj2);

  TEST("dbru_osl::add_object() ", osl1->n_objects(), 2);
  TEST("dbru_osl::n_classes() ", osl1->n_classes(), 2);
  
  TEST("dbru_osl::n_protos_in_class ", osl1->n_protos_in_class("class1"), 1);
  TEST("dbru_osl::n_protos_in_class ", osl1->n_protos_in_class("class2"), 2);

  //TEST("dbru_osl::remove_class ", osl1->remove_class("class1"), true);
  //TEST("dbru_osl::remove_class ", osl1->n_classes(), 1);
  //TEST("dbru_osl::n_protos_in_class ", osl1->n_protos_in_class("class1"), 0);

  //osl1->add_prototype("class2", obs);
  //TEST("dbru_osl::n_protos_in_class ", osl1->n_protos_in_class("class2"), 3);
  //TEST("dbru_osl::n_objects ", osl1->n_objects(), 3);
  vcl_cout << *osl1 << '\n';
  
  //  Test Binary I/O

  vsl_b_ofstream bp_out3("test_obs_io.tmp");
  obs->b_write(bp_out3);
  bp_out3.close();

  vsl_b_ifstream bp_in3("test_obs_io.tmp");
  dbinfo_observation_sptr obs_in = new dbinfo_observation();
  obs_in->b_read(bp_in3);
  vcl_cout << "Observation passed...\n";

  vsl_b_ofstream bp_out4("test_obj_io.tmp");
  obj2->b_write(bp_out4);
  bp_out4.close();

  vsl_b_ifstream bp_in4("test_obj_io.tmp");
  dbru_object_sptr obj_in = new dbru_object();
  obj_in->b_read(bp_in4);
  vcl_cout << "Object passed...\n";
  

  //Test writing the generic feature data pointer
  vsl_b_ofstream bp_out2("test_osl_io.tmp");
  osl1->b_write(bp_out2);
  //vsl_b_write(bp_out2, *osl);
  bp_out2.close();

  /*classes.push_back("class1");   classes.push_back("class2");
  vcl_vector<vcl_vector<dbinfo_observation_sptr> > prototypes(2);
  vcl_vector<dbinfo_observation_sptr> protos;
  protos.push_back(obs);
  prototypes[0]=protos;
  protos.push_back(obs);
  prototypes[1]=protos;
  dbru_osl_sptr osl = new dbru_osl(classes, prototypes);
  vcl_cout << *osl << '\n';
  //  Test Binary I/O
  //Test writing the generic feature data pointer
  vsl_b_ofstream bp_out2("test_observation_io.tmp");
  osl->b_write(bp_out2);
  //vsl_b_write(bp_out2, *osl);
  bp_out2.close();
*/

  


  vsl_b_ifstream bp_in2("test_osl_io.tmp");
  //dbru_osl_sptr osl_in = 0;
  dbru_osl_sptr osl_in = new dbru_osl();
  //osl_in->b_read(bp_in2);
  vsl_b_read(bp_in2, *osl_in);
  bp_in2.close();
  if(osl_in)
    vcl_cout << "recovered osl " << *osl_in << '\n';

  
  /*vpl_unlink ("test_osl_io.tmp");
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
  //osl_in->remove_prototype("class2", "An observation");
  //vcl_vector<dbinfo_observation_sptr> protos2;
  //good = osl_in->prototypes("class2", protos2);
  //if(good)
  //  {
  //    good = good && protos2.size()==1;
  //  }
  //TEST("Remove prototype ", good , true );
  //test remove class
  //good = osl_in->remove_class("class2");
  //if(good)
  //  {
  //    good = good && !osl_in->prototypes("class2", protos2);
  //  }
  //TEST("Remove class ", good , true );
  */
}
TESTMAIN_ARGS(test_osl);
