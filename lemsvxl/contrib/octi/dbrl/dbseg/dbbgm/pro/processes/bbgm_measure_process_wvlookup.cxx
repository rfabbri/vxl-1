		 // This is brl/bseg/bbgm/pro/processes/bbgm_measure_process.cxx
#define MEASURE_BKGROUND
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <dbbgm/bbgm_image_of.h>
#include <dbbgm/bbgm_image_sptr.h>
#include <dbbgm/bbgm_update.h>
#include <dbbgm/bbgm_interpolate.h>
#include <dbbgm/bbgm_wavelet_sptr.h>
#ifdef DEVELINCLUDEPATH
#include <dbsta/bsta_attributes.h>
#include <dbsta/bsta_gauss_if3.h>
#include <dbsta/bsta_gauss_f1.h>
#include <dbsta/bsta_mixture.h>
#include <dbsta/bsta_basic_functors.h>
#include <dbsta/algo/bsta_adaptive_updater.h>
#include <dbsta/bsta_histogram.h>
#else
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_basic_functors.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_histogram.h>
#endif
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <dbbgm/bbgm_measure.h>
#include <brip/brip_vil_float_ops.h>
#include <vcl_cstddef.h> // for std::size_t
#include <vcl_string.h> // for std::string::npos

typedef bsta_gauss_f1 bsta_gauss1_t;
typedef bsta_num_obs<bsta_gauss1_t> gauss_type1;
typedef bsta_mixture<gauss_type1> mix_gauss_type1;
typedef bsta_num_obs<mix_gauss_type1> obs_mix_gauss_type1;
typedef bbgm_image_of<obs_mix_gauss_type1> imageofgaussmix1;

typedef bsta_gauss_if3 bsta_gauss3_t;
typedef bsta_gauss3_t::vector_type vector3_;
typedef bsta_num_obs<bsta_gauss3_t> gauss_type3;
typedef bsta_mixture<gauss_type3> mix_gauss_type3;
typedef bsta_num_obs<mix_gauss_type3> obs_mix_gauss_type3;
typedef bbgm_image_of<obs_mix_gauss_type3> imageofgaussmix3;
bbgm_interp_ftr_base<imageofgaussmix1,obs_mix_gauss_type1> gray_ftr;
bbgm_interp_ftr_base<imageofgaussmix3,obs_mix_gauss_type3> color_ftr;



bool bbgm_measure_wvlookup_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> in_types(8), out_types(1);
  in_types[0]= "bbgm_wavelet_sptr"; //wavelet transform background image
  in_types[1]= "vil_image_view_base_sptr"; //test image
  in_types[2]= "vcl_string"; //what to measure
  in_types[3]= "float"; // measure tolerance
  in_types[4]="vcl_string"; //which functor
  in_types[5]="vcl_string";//data path
  in_types[6]="vcl_string";//frame suffix
  in_types[7]="float";//threshold
  pro.set_input_types(in_types);
  out_types[0]= "vil_image_view_base_sptr"; //the probability map
  
  pro.set_output_types(out_types);
  return true;
}

//: Execute the process function
bool bbgm_measure_wvlookup_process(bprb_func_process& pro)
{

  // Sanity check
  if (!pro.verify_inputs()) {
    vcl_cerr << "In bbgm_measure_process::execute() -"
             << " invalid inputs\n";
    return false;
  }

  // Retrieve background image

  bbgm_wavelet_sptr wavelet_ptr = pro.get_input<bbgm_wavelet_sptr>(0);
  if (!wavelet_ptr) {
    vcl_cerr << "In bbgm_measure_process::execute() -"
             << " null wavelet model image\n";
    return false;
  }
 
  vil_image_view_base_sptr img_ptr = 
    pro.get_input<vil_image_view_base_sptr>(1);
  if (!img_ptr) {
    vcl_cerr << "In bbgm_measure_process::execute() -"
             << " null measurement input image\n";
    return false;
  }
  vil_image_view<float> image = *vil_convert_cast(float(), img_ptr);
  if (img_ptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(image,1.0/255.0);

  unsigned np = image.nplanes();

  //Retrieve attribute to measure, e.g. probability
  vcl_string attr = pro.get_input<vcl_string>(2);
  vcl_string interp_ftr_str=pro.get_input<vcl_string>(4);
  vcl_string data_path=pro.get_input<vcl_string>(5);
  vcl_string frame_suffix=pro.get_input<vcl_string>(6);
  float threshold=pro.get_input<float>(7);
  //Retrieve measure tolerance
  float tolerance = pro.get_input<float>(3);

  vil_image_view<float> result;
/*  if (np ==1) {
   
	
	 bbgm_interp_bicubic_ftr<imageofgaussmix1,obs_mix_gauss_type1> interp_ftr_cub;
	 bbgm_interp_bilinear_ftr<imageofgaussmix1,obs_mix_gauss_type1> interp_ftr_bil;
	 bbgm_interp_nearest_ftr<imageofgaussmix1,obs_mix_gauss_type1> interp_ftr_nn;
	 bbgm_interp_lanczos_ftr<imageofgaussmix1,obs_mix_gauss_type1> interp_ftr_lcz;
		    if (attr=="probability") {
      bsta_probability_functor<mix_gauss_type1> functor_;
      measure(*model, image, result, functor_, tolerance);
    }
#ifdef MEASURE_BKGROUND
    else if (attr=="prob_background") {
      bsta_prob_density_functor<mix_gauss_type1> functor_;
      measure_bkground(*model, image, result, functor_, tolerance);
    }
#endif // MEASURE_BKGROUND
    else {
      vcl_cout << "In bbgm_measure_process::execute() -"
               << " measurement not available\n";
      return false;
    }
  }*/
  if (np ==3)
  {
	  if(interp_ftr_str=="nearest")	
		color_ftr=bbgm_interp_nearest_ftr<imageofgaussmix3,obs_mix_gauss_type3>();
	  else if(interp_ftr_str=="bil")
        color_ftr=bbgm_interp_bilinear_ftr<imageofgaussmix3,obs_mix_gauss_type3>();
	  else if(interp_ftr_str=="cubic")
		  color_ftr=bbgm_interp_bicubic_ftr<imageofgaussmix3,obs_mix_gauss_type3>();
	  else if(interp_ftr_str=="lancz") 
		  color_ftr=bbgm_interp_lanczos_ftr<imageofgaussmix3,obs_mix_gauss_type3>();
	  else {
	  	  vcl_cerr << "In bbgm_measure_process::execute() -"
             << " invalid interp functor; must be one of nearest ,bil, cubic or lancz\n";
		  return false;
	  }
    
    if (attr=="probability") {
        bsta_probability_functor<mix_gauss_type3> functor_;
       	bbgm_wavelet<imageofgaussmix3> *wavelet=static_cast<bbgm_wavelet<imageofgaussmix3> *>(wavelet_ptr.ptr());
		measure_wv_lookup(*wavelet, image, result, functor_,color_ftr, tolerance,data_path,frame_suffix,threshold);
      
    }
/*#ifdef MEASURE_BKGROUND
    else if (attr=="prob_background") {
        bbgm_image_of<obs_mix_gauss_type3> *model =
          static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(bgm.ptr());
        bsta_prob_density_functor<mix_gauss_type3> functor_;
        measure_bkground(*model, image, result, functor_, tolerance);
     
    }
#endif // MEASURE_BKGROUND	 */
    else {
      vcl_cout << "In bbgm_measure_process::execute() -"
               << " measurement not available\n";
      return false;
    }
  }
  vcl_vector<vcl_string> output_types(1);
  output_types[0]= "vil_image_view_base_sptr";
  pro.set_output_types(output_types);
  brdb_value_sptr output =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(result));
  pro.set_output(0, output);

  return true;
}

