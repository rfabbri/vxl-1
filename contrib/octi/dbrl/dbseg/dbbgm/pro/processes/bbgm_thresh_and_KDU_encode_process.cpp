#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_threshold.h>
#include <vil/vil_math.h>
#include <dkdu/apps/kdu_compress/compress_local.h>
#include <vil/vil_save.h>
//static bool stringToArgv(vcl_vector<char*>& args,vcl_istringstream& iss,int &argc);
bool stringToArgv(vcl_vector<char*>& args,vcl_istringstream& iss,int &argc);
bool bbgm_thresh_and_KDU_encode_process_cons(bprb_func_process& pro)
{
    vcl_vector<vcl_string> in_types(6);
  in_types[0]="vil_image_view_base_sptr";//input image
  in_types[1]="vil_image_view_base_sptr";//input image
  in_types[2]="float";//bg thresh
  in_types[3]="vcl_string";//argv
  in_types[4]="int";//roi coding
  in_types[5]="vcl_string";// kdu args for frame
  pro.set_input_types(in_types);
  return true;

}

bool bbgm_thresh_and_KDU_encode_process(bprb_func_process& pro)
{
   
   if (!pro.verify_inputs()) {
    vcl_cerr << "In bbgm_thresh_and_compress_process::execute() -"
             << " invalid inputs\n";
    return false;
  }	
  vil_image_view_base_sptr truth_img_ptr = 
    pro.get_input<vil_image_view_base_sptr>(0);
  if (!truth_img_ptr) {
    vcl_cerr << "In bbgm_thresh_and_compress_process::execute() -"
             << " null probability map\n";
    return false;
  }

   vil_image_view_base_sptr input_img_ptr = 
    pro.get_input<vil_image_view_base_sptr>(1);
  if (!input_img_ptr) {
    vcl_cerr << "In bbgm_thresh_and_compress_process::execute() -"
             << " null measurement input image\n";
    return false;
  }
  vil_image_view<float> truth = * vil_convert_cast(float(), truth_img_ptr);
  vil_image_view<vxl_byte> input = * vil_convert_cast(vxl_byte(), input_img_ptr);
  vil_image_view<bool> foreground_map=new vil_image_view<bool>(input.ni(),input.nj(),1,1);
  vil_image_view<vxl_byte> output_frame=new vil_image_view<vxl_byte>(input.ni(),input.nj(),input.nplanes(),1);
  
  float bg_thresh=pro.get_input<float>(2);
  vil_threshold_below(truth,foreground_map,1-bg_thresh);
  
    
  vil_math_image_product(input,foreground_map,output_frame);

  int roi=pro.get_input<int>(4);
  vil_image_view<vxl_byte> hi_dynam=new vil_image_view<vxl_byte>(input.ni(),input.nj(),1,1);
  vil_image_view<vxl_byte> mask=new vil_image_view<vxl_byte>(input.ni(),input.nj(),1,1);
  hi_dynam.fill(vxl_byte(255));
  vil_math_image_product(hi_dynam,foreground_map,mask);
  
  if(roi){
	vil_save(mask,"C:\\Temp\\roi.pgm");
	vil_save(mask,"C:\\Temp\\roi.bmp"); 
  }
  vcl_string argv=pro.get_input<vcl_string>(3);
  vcl_string argv_f=pro.get_input<vcl_string>(5);
  vcl_istringstream istr(argv), istr_m(argv_f);
  
  vcl_vector<char*> args,mask_args;	  
  int argc,argc_m;
  bool success=stringToArgv(args,istr,argc);
  bool suc2=stringToArgv(mask_args,istr_m,argc_m);
  if (roi){
	args.push_back("-roi");
	args.push_back("C:\\Temp\\roi.pgm,0.5");
	argc+=2;
  }
  vil_math_scale_values(mask,255.0);
  if (suc2)
	  suc2=kdu_compress_vil_image(mask,argc_m,&mask_args[0]);
  if (success)
	success=kdu_compress_vil_image(output_frame,argc,&args[0]);
  return success;
}