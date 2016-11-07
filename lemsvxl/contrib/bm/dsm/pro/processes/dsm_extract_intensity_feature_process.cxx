//this is /contrib/bm/dsm/pro/processes/dsm_extract_intensity_feature_process.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include"dsm/dsm_feature_sptr.h"

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

namespace dsm_extract_intensity_feature_process_globals
{
	const unsigned int n_inputs_ = 4;
	const unsigned int n_outputs_ = 1;
}

//set input/output types
bool dsm_extract_intensity_feature_process_cons(bprb_func_process& pro)
{
	using namespace dsm_extract_intensity_feature_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);
	
	unsigned i = 0;
	input_types_[i++] = "vil_image_view_base_sptr";// the target image
	input_types_[i++] = "unsigned"; //the x image coordinate
	input_types_[i++] = "unsigned"; //the y image coordinate
	input_types_[i++] = "unsigned"; //time

	input_types_[0] = "dsm_feature_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}

bool dsm_extract_intensity_feature_process(bprb_func_process& pro)
{
	using namespace dsm_extract_intensity_feature_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_extract_intensity_feature_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	vil_image_view_base_sptr vil_base_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
	unsigned key_x = pro.get_input<unsigned>(i++);
	unsigned key_y = pro.get_input<unsigned>(i++);
	unsigned time = pro.get_input<unsigned>(i++);

	if( vil_base_sptr->nplanes() > 1 || vil_base_sptr->pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE )
	{
		vcl_cout << "---- ERROR: dsm_extract_intensity_feature_process ----\n"
			     << "\t Please provide a grey scale image. " << vcl_endl;
		return false;
	}

	vil_image_view<vxl_byte> img_view(vil_base_sptr);

	dsm_feature_sptr feature_sptr = new dsm_feature(vnl_vector<double>(1,img_view(key_x,key_y)), time);

	pro.set_output_val(0,feature_sptr);

	return true;
}