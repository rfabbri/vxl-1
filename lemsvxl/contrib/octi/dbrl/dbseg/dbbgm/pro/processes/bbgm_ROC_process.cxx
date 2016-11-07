#include <dbbgm/bbgm_ROC.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
bool bbgm_ROC_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> in_types(4);
  in_types[0]="vil_image_view_base_sptr";//truth
  in_types[1]="vil_image_view_base_sptr";//test
  in_types[2]="float";//bg thresh
  in_types[3]="vcl_string"; //path
  pro.set_input_types(in_types);
  return true;

}

bool bbgm_ROC_process(bprb_func_process& pro)
{
   
   if (!pro.verify_inputs()) {
    vcl_cerr << "In bbgm_ROC_process::execute() -"
             << " invalid inputs\n";
    return false;
  }	
  vil_image_view_base_sptr truth_img_ptr = 
    pro.get_input<vil_image_view_base_sptr>(0);
  if (!truth_img_ptr) {
    vcl_cerr << "In bbgm_ROC_process::execute() -"
             << " null measurement input image\n";
    return false;
  }
  vil_image_view<float> truth = * vil_convert_cast(float(), truth_img_ptr);

  vil_image_view_base_sptr test_img_ptr = 
    pro.get_input<vil_image_view_base_sptr>(1);
  if (!test_img_ptr) {
    vcl_cerr << "In bbgm_ROC_process::execute() -"
             << " null measurement input image\n";
    return false;
  }
  vil_image_view<float> test = *vil_convert_cast(float(), test_img_ptr);
  vcl_string path=pro.get_input<vcl_string>(3);
  float bg_thresh=pro.get_input<float>(2);
  specificitySensitivityCount(truth,test,bg_thresh,path);
  return true;
}