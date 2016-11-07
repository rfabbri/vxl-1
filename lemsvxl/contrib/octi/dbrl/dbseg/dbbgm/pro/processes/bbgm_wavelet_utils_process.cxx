#include <vcl_iostream.h>
#include <dbbgm/bbgm_image_of.h>
#include <dbbgm/bbgm_image_sptr.h>
#include <dbbgm/bbgm_wavelet.h>
#include <dbbgm/bbgm_wavelet_sptr.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
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

bool bbgm_wavelet_utils_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> in_types(5), out_types(1);
  in_types[0]= "vcl_string"; //type
  in_types[1]="int"; //nplanes
  in_types[2]= "vcl_string";//the subband. must be LL,LH,HL,HH,inverse or forward.
  in_types[3]= "int"; //the depth
  in_types[4]= "bbgm_wavelet_sptr"; // the actual image.
  pro.set_input_types(in_types);
  return true;
}

bool bbgm_wavelet_utils_process(bprb_func_process& pro)
{

	if (!pro.verify_inputs()) {
    vcl_cerr << "In bbgm_wavelet_utils_process::execute() -"
             << " invalid inputs\n";
    return false;
    }
	
	vcl_string type=pro.get_input<vcl_string>(0);
	int nplanes=pro.get_input<int>(1);
	vcl_string subband_type=pro.get_input<vcl_string>(2);
	int depth=pro.get_input<int>(3);
	bbgm_wavelet_sptr input_wavelet = pro.get_input<bbgm_wavelet_sptr>(4);
	if(!input_wavelet) {
	   vcl_cerr << "In bbgm_wavelet_utils_process::execute() -"
	   << " invalid subband\n";
	   return false;
	}
	
	subband current_band;
	
	if (subband_type=="LL")
		current_band=LL;
	else if(subband_type=="LH")
		current_band=LH;
	else if(subband_type=="HL")
		current_band=HL;
	else if(subband_type=="HH")
		current_band=HH;
	else if	(subband_type=="inverse");
	else if (subband_type=="forward");
	else{
		vcl_cerr << "In bbgm_wavelet_utils_process::execute() -"
             << " invalid subband; must be LL,LH,HL,HH,inverse or forward\n";
		return false;
	}  

	if (type=="bbgm_image_sptr"){
		
		bbgm_image_sptr output_model;
		if (nplanes ==3)
			{
				bbgm_wavelet<bbgm_image_of<obs_mix_gauss_type3> > *wavelet =
					static_cast<bbgm_wavelet<bbgm_image_of<obs_mix_gauss_type3> > *> (input_wavelet.ptr());
				
				if (subband_type=="inverse")
					output_model =new bbgm_image_of<obs_mix_gauss_type3>(*wavelet->spatialDomain());
				else if (subband_type=="forward")
					output_model =wavelet->waveletDomain();
				else
					output_model =wavelet->getSubband(current_band,depth);
		   	 }
		 //commit the output
		 vcl_vector<vcl_string> output_types(1);
		 output_types[0]= "bbgm_image_sptr";
         pro.set_output_types(output_types);
         brdb_value_sptr output =
            new brdb_value_t<bbgm_image_sptr>(output_model);
		 pro.set_output(0, output);

		}else if(type=="vil_image_view_base_sptr"){
			vil_image_view_base_sptr output_image;
			bbgm_wavelet<vil_image_view<float> > *image =
					static_cast<bbgm_wavelet<vil_image_view<float> >* > (input_wavelet.ptr());
			if (subband_type=="inverse")
				output_image=new vil_image_view<float> (*image->spatialDomain());
			else
				output_image =image->getSubband(current_band,depth);

			vcl_vector<vcl_string> output_types(1);
			output_types[0]= "vil_image_view_base_sptr";
			pro.set_output_types(output_types);
			brdb_value_sptr output =
            new brdb_value_t<vil_image_view_base_sptr>(output_image);
			pro.set_output(0, output);

		}
	return true;
}