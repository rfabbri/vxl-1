 #include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_threshold.h>
#include <vil/vil_math.h>
#include <dkdu/apps/kdu_expand/kdu_expand_bg.h>
//static bool stringToArgv(std::vector<char*>& args,std::istringstream& iss,int &argc);
bool stringToArgv(std::vector<char*>& args,std::istringstream& iss,int &argc);
bool bbgm_KDU_decompress_and_glue_process_cons(bprb_func_process& pro)
{
    std::vector<std::string> in_types(3);
  in_types[0]="vil_image_view_base_sptr";//background image
  in_types[1]=vcl_string";//argv
  in_types[2]=vcl_string";//mask argv
  pro.set_input_types(in_types);

  return true;

}

bool bbgm_KDU_decompress_and_glue_process(bprb_func_process& pro)
{
   
   if (!pro.verify_inputs()) {
    std::cerr << "In bbgm_thresh_and_compress_process::execute() -"
             << " invalid inputs\n";
    return false;
  }	
  vil_image_view_base_sptr input_img_ptr = 
    pro.get_input<vil_image_view_base_sptr>(0);
  if (!input_img_ptr) {
    std::cerr << "In bbgm_thresh_and_compress_process::execute() -"
             << " null probability map\n";
    return false;
  }

  vil_image_view<vxl_byte> bg_img = * vil_convert_cast(vxl_byte(), input_img_ptr);
  vil_image_view<vxl_byte> output_img,mask_img;
  std::string argv=pro.get_input<std::string>(1);
  std::string argv_m=pro.get_input<std::string>(2);
  std::istringstream istr(argv);
  std::istringstream istr_m(argv_m);
  std::vector<char*> args,args_m;	  
  int argc,argc_m;
  bool success=stringToArgv(args,istr,argc);
  bool suc2=stringToArgv(args_m,istr_m,argc_m);

  vil_image_view<vxl_byte> interm_foreground;
  if (suc2){
	  suc2=kde_decompress_vil_image(mask_img,argc_m,&args_m[0]);
	   interm_foreground=new vil_image_view<vxl_byte>
	  (bg_img.ni(),bg_img.nj(),bg_img.nplanes(),1);
  }
  
  if (success)
	success=kde_decompress_vil_image(output_img,argc,&args[0]);
  
  vil_image_view<vxl_byte> intermediate_frame=new vil_image_view<vxl_byte>
	  (bg_img.ni(),bg_img.nj(),bg_img.nplanes(),1);
  vil_image_view<vxl_byte> final_frame=new vil_image_view<vxl_byte>
	  (bg_img.ni(),bg_img.nj(),bg_img.nplanes(),1);
  
  vil_image_view<bool> foreground_map=new vil_image_view<bool>
	  (output_img.ni(),output_img.nj(),output_img.nplanes(),1);
  
  if (suc2){
	vil_math_image_product(output_img,mask_img,interm_foreground);
	vil_math_image_product(bg_img,mask_img,intermediate_frame);
	output_img=interm_foreground;
  }else{
	vil_threshold_below(output_img,foreground_map,static_cast<vxl_byte>(1));
	vil_math_image_product(bg_img,foreground_map,intermediate_frame);
  }
 
  vil_math_image_sum(intermediate_frame,output_img,final_frame);
  vil_image_view_base_sptr output_img_sptr=new vil_image_view<vxl_byte>(output_img);
  vil_image_view_base_sptr final_img_sptr=new vil_image_view<vxl_byte>(final_frame);
  
 // vil_image_view<vxl_byte> byte_output = * vil_convert_cast(vxl_byte(), output_img_sptr);
 // vil_image_view<vxl_byte> byte_final = * vil_convert_cast(vxl_byte(), final_img_sptr); 
  
  std::vector<std::string> output_types(2);
  output_types[0]= "vil_image_view_base_sptr";
  output_types[1]= "vil_image_view_base_sptr";
  pro.set_output_types(output_types);
  
  brdb_value_sptr output =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(output_img_sptr));
  
  brdb_value_sptr output2 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(final_frame));
 
  pro.set_output(1, output2);
  pro.set_output(0, output);
  return success;
}