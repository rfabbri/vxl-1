#include <testlib/testlib_test.h>
#include <dborl/dborl_image_object.h>
#include <dborl/dborl_image_object_sptr.h>
#include <dborl/dborl_image_bbox_description_sptr.h>
#include <dborl/dborl_image_bbox_description.h>
#include <dborl/dborl_image_description.h>
#include <dborl/dborl_dataset.h>
#include <dborl/dborl_dataset_sptr.h>
#include <vcl_iostream.h>
//#include <vcl_cmath.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vsol/vsol_box_2d.h>

MAIN_ARGS(test_dataset)
{
  testlib_test_start("testing dataset class ");
 
  const unsigned int ni = 55;
  const unsigned int nj = 35;
  vil_image_view<float> image;
  image.set_size(ni,nj);  image.fill(17.f);
  vil_image_resource_sptr img = vil_new_image_resource_of_view(image);

  dborl_image_object_sptr io = new dborl_image_object("test_image", img);
  
  dborl_image_bbox_description_sptr box_d = new dborl_image_bbox_description();
  vsol_box_2d_sptr b = new vsol_box_2d();
  b->add_point(10, 10);
  b->add_point(20, 20);
  box_d->add_box("test_cat", b);

  dborl_image_description_sptr img_d = new dborl_image_description(box_d);
  io->set_description(img_d);

  dborl_dataset_sptr ds = new dborl_dataset();
  TEST("test dataset constructor ", ds->size(), 0);

  ds->add_object(io->cast_to_object_base());
  TEST("test dataset size() ", ds->size(), 1);

  vil_image_view<float> image2;
  image2.set_size(ni,nj);  image2.fill(19.f);
  vil_image_resource_sptr img2 = vil_new_image_resource_of_view(image2);

  dborl_image_object_sptr io2 = new dborl_image_object("test_image2", img2);
  dborl_image_bbox_description_sptr box_d2 = new dborl_image_bbox_description();
  vsol_box_2d_sptr b2 = new vsol_box_2d();
  b2->add_point(15, 12);
  b2->add_point(22, 23);
  box_d2->add_box("test_cat", b2);

  vsol_box_2d_sptr b3 = new vsol_box_2d();
  b3->add_point(5, 7);
  b3->add_point(2, 9);
  box_d2->add_box("test_cat2", b3);

  dborl_image_description_sptr img_d2 = new dborl_image_description(box_d2);
  io2->set_description(img_d2);

  ds->add_object(io2->cast_to_object_base());
  TEST("test dataset size() ", ds->size(), 2);

  vcl_map<vcl_string, int> cnts;
  ds->get_category_cnts(cnts);
  TEST("test dataset get_category_cnts() ", cnts.size(), 2);
  TEST("test dataset get_category_cnts() ", cnts["test_cat"], 2);
  TEST("test dataset get_category_cnts() ", cnts["test_cat2"], 1);

  vcl_map<vcl_string, vcl_vector<dborl_object_base_sptr> > assign;
  ds->get_category_assignments(assign);
  TEST("test dataset get_category_assignments() ", assign.size(), 2);
  TEST("test dataset get_category_assignments() ", assign["test_cat"].size(), 2);
  TEST("test dataset get_category_assignments() ", assign["test_cat2"].size(), 1);

  vcl_vector<dborl_object_base_sptr>& objs = assign["test_cat"];
  TEST("test dataset get_category_assignments() ", !(objs[0]->cast_to_image_object()), false);
  TEST("test dataset get_category_assignments() ", objs[0]->cast_to_image_object()->desc_->category_list_.size(), 1);
  TEST("test dataset get_category_assignments() ", objs[1]->cast_to_image_object()->desc_->category_list_.size(), 2);
  return testlib_test_summary();
}
