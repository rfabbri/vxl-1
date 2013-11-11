#include <testlib/testlib_test.h>
#include <dborl/dborl_image_object.h>
#include <dborl/dborl_image_object_sptr.h>
#include <dborl/dborl_image_bbox_description_sptr.h>
#include <dborl/dborl_image_bbox_description.h>
#include <dborl/dborl_image_description.h>
#include <vcl_iostream.h>
//#include <vcl_cmath.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vsol/vsol_box_2d.h>

static void test_image_object(int argc, char* argv[])
//MAIN_ARGS(test_image_object)
{
  testlib_test_start("testing image_object class ");
 
  vil_image_resource_sptr img = vil_load_image_resource("pascal_000005.jpg");
  if (!img) {
    const unsigned int ni = 55;
    const unsigned int nj = 35;
    vil_image_view<float> image;
    image.set_size(ni,nj);  image.fill(17.f);
    img = vil_new_image_resource_of_view(image);  
  }

  vcl_cout << "img size: " << img->ni() << " " << img->nj() << vcl_endl;

  dborl_image_object_sptr io = new dborl_image_object("test_image", img);
  TEST("test constructor ", io->get_object_type(), dborl_object_type::image);

  dborl_image_bbox_description_sptr box_d = new dborl_image_bbox_description();

  TEST("test category_exists() ", box_d->category_exists("test_cat"), false);
  vsol_box_2d_sptr b = new vsol_box_2d();
  b->add_point(10, 10);
  b->add_point(20, 20);
  box_d->add_box("test_cat", b);
  TEST("test category_exists() ", box_d->category_exists("test_cat"), true);

  vcl_vector<vsol_box_2d_sptr>& v = box_d->get_box_vector("test_cat");
  TEST("test get_box_vector() ", v.size(), 1);
  TEST("test get_box_vector() ", v[0]->area(), b->area());

  dborl_image_description_sptr img_d = new dborl_image_description(box_d);
  io->set_description(img_d);

  TEST("test set_description() get_description()", io->get_description()->category_exists("test_cat"), true);
  TEST("test set_description() get_description()", io->get_description()->category_data_->cast_to_image_bbox_description()->category_exists("test_cat"), true);

  TEST("test set_description() get_description()", io->get_description()->category_exists("dummy"), false);
  TEST("test set_description() get_description()", io->get_description()->category_data_->cast_to_image_bbox_description()->category_exists("dummy"), false);

  TEST("test cast_to_object_base() ", !(io->cast_to_object_base()), false);
  TEST("test cast_to_image_object() ", !(io->cast_to_image_object()), false);

  //return testlib_test_summary();
}

TESTMAIN_ARGS(test_image_object)
