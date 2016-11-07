#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/vil_new.h>
#include <imgr/file_formats/imgr_ff_algos.h>

static void test_imgr_ff_algos()
{
  unsigned ni = 11, nj = 11;
  unsigned ib0 = 3, jb0=4;
  unsigned ib1 = 7, jb1 = 8;

  double min_pos[2]={ib0,jb0}, max_pos[2]={ib1, jb1};


  //Make a 2-d view
  vil_image_view<unsigned short> *view_tmp = new vil_image_view<unsigned short>(ni, nj);
  view_tmp->fill(10);
  vil_image_resource_sptr v = vil_new_image_resource_of_view(*view_tmp);

  vcl_cout << v;
  
  vil_image_view<unsigned short> *dark_tmp = new vil_image_view<unsigned short>(ni, nj);
  dark_tmp->fill(5);
  vil_image_resource_sptr dark = vil_new_image_resource_of_view(*dark_tmp);
  vcl_cout << dark;

  vil_image_view<unsigned short> *bright_tmp = new vil_image_view<unsigned short>(ni, nj);
  bright_tmp->fill(100);
  vil_image_resource_sptr bright = vil_new_image_resource_of_view(*bright_tmp);
  
  //construct a bounded 2-d view
  vgl_box_2d<double> box(min_pos, max_pos);

  dbil_bounded_image_view<double>* bv = 
    imgr_ff_algos:: calibrate_image(v, dark, bright, box);
 
  TEST_NEAR("test imgr_ff_algos ",  bv->operator()(0,0), -vcl_log(5.0/95), 1e-6);
  
}
TESTMAIN(test_imgr_ff_algos);
