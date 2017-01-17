#include <testlib/testlib_test.h>
#include <dborl/dborl_category_info.h>
#include <dborl/dborl_category_info_sptr.h>

#include <vcl_iostream.h>
//#include <vcl_cmath.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vsol/vsol_box_2d.h>

MAIN_ARGS(test_category_info)
{
  testlib_test_start("testing category_info class ");
 
  dborl_category_info_sptr ci = new dborl_category_info();
  TEST("test category_info constructor ", ci->prefix_list_.size(), 0);
  ci->set_name("test1");
  ci->add_prefix("test");
  TEST("test category_info constructor ", ci->prefix_list_.size(), 1);
  
  dborl_category_info_sptr ci2 = new dborl_category_info("test22");
  vcl_vector<vcl_string> prefixes;
  prefixes.push_back("test2");
  prefixes.push_back("test3");
  prefixes.push_back("test4");
  ci2->set_prefix_list(prefixes);
  TEST("test category_info constructor ", ci2->name_.compare("test22"), 0);
  TEST("test category_info constructor ", ci2->prefix_list_.size(), 3);
  vil_rgb<vxl_byte> n(255, 100, 3);
  ci2->set_color(n);
  TEST("test category_info constructor ", ci2->color_.R(), 255);
  TEST("test category_info constructor ", ci2->color_.G(), 100);
  TEST("test category_info constructor ", ci2->color_.B(), 3);

  return testlib_test_summary();
}
