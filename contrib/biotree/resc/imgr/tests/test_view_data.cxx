#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <imgr/imgr_view_data_base.h>
#include <dbil/dbil_bounded_image_view.h>
static void test_view_data()
{
  //test dummy_scan
  vcl_cout << "===========Testing view data ========\n";

  //A 2-d view to put in the viewdata block
  vil_image_view<unsigned short>* nv = 
    new vil_image_view<unsigned short>(10,10);
  nv->fill(100);
  vil_image_view_base_sptr nvp = nv;

  //A new empty view
  imgr_view_data_base_sptr vd = new imgr_view_data_base();
  //Put the view_base_sptr in the view_data
  vd->add_view_2d(nv);
  //Get it back out
  vil_image_view<unsigned short>  rv = vd->view_2d(0);
  //See if it survived
  vcl_cout << rv.ni() << '\n';
  vcl_cout << rv.pixel_format() << '\n';
  bool good = rv.ni()==10&&rv(5,5)==100;
  TEST("test cast", good, true);
}
TESTMAIN(test_view_data);
