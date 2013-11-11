//this is /contrib/bm/dsm/pro/procesess/dsm_extract_bapl_dsift_feature_process.cxx
#include<bapl/bapl_dsift_sptr.h>

#include<bprb/bprb_func_process.h>
#include<brdb/brdb_value.h>

#include"dsm/dsm_feature_sptr.h"

#include"vil/vil_image_resource_sptr.h"
#include"vil/vil_image_view_base.h"

namespace dsm_extract_bapl_dsift_feature_process_globals
{
	const unsigned int n_inputs_ = 4;
	const unsigned int n_outputs_ = 1;
}//end dsm_extract_sift_vil_process_globals

//set input/output types
bool dsm_extract_bapl_dsift_feature_process_cons(bprb_func_process& pro)
{
	using namespace dsm_extract_bapl_dsift_feature_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	unsigned i = 0;
	input_types_[i++] = "bapl_dsift_sptr";
	input_types_[i++] = "unsigned"; //x image coordinate
	input_types_[i++] = "unsigned"; //y image coordinate
	input_types_[i++] = "unsigned"; //time

	output_types_[0] = "dsm_feature_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}//end dsm_extract_sift_vil_process_cons

bool dsm_extract_bapl_dsift_feature_process(bprb_func_process& pro)
{
	using namespace dsm_extract_bapl_dsift_feature_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_extract_bapl_dsift_feature_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned  i = 0;
	bapl_dsift_sptr dsift_sptr = pro.get_input<bapl_dsift_sptr>(i++);
	unsigned key_x = pro.get_input<unsigned>(i++);
	unsigned key_y = pro.get_input<unsigned>(i++);
	unsigned time = pro.get_input<unsigned>(i++);

	dsm_feature_sptr feature_sptr = new dsm_feature(dsift_sptr->vnl_dsift(key_x,key_y),time);		

	pro.set_output_val(0,feature_sptr);

	return true;
}//end dsm_extract_sift_vil_process

