#include <iostream>
#include <dbbgm/bbgm_image_of.h>
#include <dbbgm/bbgm_image_sptr.h>
#include <dbbgm/bbgm_wavelet_sptr.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <iostream>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <dbsta/bsta_attributes.h>
#include <dbsta/bsta_gauss_if3.h>
#include <dbsta/bsta_gauss_f1.h>
#include <dbsta/bsta_mixture.h>
#include <dbsta/bsta_basic_functors.h>
#include <dbsta/algo/bsta_adaptive_updater.h>
#include <dbsta/bsta_histogram.h>

typedef bsta_gauss_f1 bsta_gauss1_t;
typedef bsta_num_obs<bsta_gauss1_t> gauss_type1;
typedef bsta_mixture<gauss_type1> mix_gauss_type1;
typedef bsta_num_obs<mix_gauss_type1> obs_mix_gauss_type1;

typedef bsta_gauss_if3 bsta_gauss3_t;
typedef bsta_gauss3_t::vector_type vector3_;
typedef bsta_num_obs<bsta_gauss3_t> gauss_type3;
typedef bsta_mixture<gauss_type3> mix_gauss_type3;
typedef bsta_num_obs<mix_gauss_type3> obs_mix_gauss_type3;
//: Process construct function//
bool stringToArgv(std::vector<char*>& args,std::istringstream& iss,int &argc);
bool bbgm_wavelet_xform_set_inputs(bprb_func_process& pro)
{
  std::vector<std::string> in_types(6);
  in_types[0]=vcl_string";//type of image
  in_types[1]="int";//wavelet number
  in_types[2]="int";//level
  in_types[3]=pro.get_input<std::string>(0); //in image
  in_types[4]="int";//number of planes
  in_types[5]=vcl_string";
  pro.set_input_types(in_types);
  return true;
}

//: Process exectute function

bool bbgm_wavelet_xform_process(bprb_func_process& pro)
{

	// Sanity check
	bbgm_wavelet_xform_set_inputs(pro);
	if (!pro.verify_inputs()) {
		std::cerr << "In bbgm_wavelet_xform_process::execute - invalid inputs\n";
		return false;
	}
   
	std::vector<std::string> output_types(2);
    output_types[0]= "bbgm_wavelet_sptr";
  
	std::string type=pro.get_input<std::string>(0);
	int waveletNo=pro.get_input<int>(1);
	int level=pro.get_input<int>(2);
	int nplanes=pro.get_input<int>(4);
	std::string arg_string=pro.get_input<std::string>(5);
	std::istringstream istr(arg_string);
	int argc=0;
	std::vector<char*> args;
	bool success=stringToArgv(args,istr,argc);

	bbgm_wavelet_sptr output_wavelet;
	brdb_value_sptr model_output;
	
	if (type=="bbgm_image_sptr"){
		bbgm_image_sptr input_image = pro.get_input<bbgm_image_sptr>(3);
		bbgm_image_sptr output_image;
		
		
		if (!input_image) {
			std::cerr << "Null background image\n";
			return false;
		}
			if (nplanes ==3)
			{
				bbgm_image_of<obs_mix_gauss_type3> *input_model =
					static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(input_image.ptr());

				output_image =
					new bbgm_image_of<obs_mix_gauss_type3>(input_model->ni(),input_model->nj(),obs_mix_gauss_type3());

				bbgm_image_of<obs_mix_gauss_type3> *output_model =
					static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(output_image.ptr());
				
				if (success)
					output_wavelet= new bbgm_wavelet<bbgm_image_of<obs_mix_gauss_type3> >(input_model,output_model,level,waveletNo,KDU,argc,&args[0]);
				else
					output_wavelet= new bbgm_wavelet<bbgm_image_of<obs_mix_gauss_type3> >(input_model,output_model,level,waveletNo);

			}
			output_types[1]= "bbgm_image_sptr";
			model_output=new brdb_value_t<bbgm_image_sptr>(output_image);

		}else if(type=="vil_image_view_base_sptr"){
			vil_image_view_base_sptr input_image = 
				pro.get_input<vil_image_view_base_sptr>(3);

			vil_image_view_base_sptr img_s = vil_convert_cast(float(), input_image);
			vil_image_view<float>* img=static_cast<vil_image_view<float>* >(img_s.ptr()); 
			if (input_image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
				vil_math_scale_values(*img,1.0/255.0);

			vil_image_view_base_sptr output_image =new vil_image_view<float>(img->ni(),img->nj(),img->nplanes(),1);  
		   	vil_image_view_base_sptr out_img_s = vil_convert_cast(float(), output_image);
			vil_image_view<float>* out_img=static_cast<vil_image_view<float>* >(out_img_s.ptr()); 
			output_wavelet=new bbgm_wavelet<vil_image_view<float> >(img,out_img,level,waveletNo);

			output_types[1]= "vil_image_view_base_sptr";
			model_output=new brdb_value_t<vil_image_view_base_sptr>(output_image);
		}
	
  
  pro.set_output_types(output_types);
  brdb_value_sptr output =
    new brdb_value_t<bbgm_wavelet_sptr>(output_wavelet);
  pro.set_output(0, output);
  pro.set_output(1,model_output);
  return true;
}

bool stringToArgv(std::vector<char*>& args,std::istringstream& iss,int &argc)
{
	argc=0;
	std::string token;
	;
	 while(iss >> token) 
	{  
	char *arg = new char[token.size() + 1];  
	copy(token.begin(), token.end(), arg);  
	arg[token.size()] = '\0';  
	args.push_back(arg);
	argc++;
	}
	return (argc!=0);		



}
