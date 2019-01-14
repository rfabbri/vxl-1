//this is /contrib/bm/dsm/pro/processes/dsm_extract_intensity_ratio_features.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_features_sptr.h>

#include<iostream>
#include<vector>
#include<string>

namespace dsm_extract_intensity_ratio_features_globals
{
	const unsigned int n_inputs_ = 1;
	const unsigned int n_outputs_ = 1;
}

bool dsm_extract_intensity_ratio_features_process_cons(bprb_func_process& pro)
{
	using namespace dsm_extract_intensity_ratio_features_globals;

	std::vector<std::string> input_types_(n_inputs_);
	std::vector<std::string> output_types_(n_outputs_);
	input_types_[0] = vcl_string";

	output_types_[0] = "dsm_features_sptr";

		if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}//end dsm_extract_intensity_ratio_process_cons

bool dsm_extract_intensity_ratio_features_process(bprb_func_process& pro)
{
	using namespace dsm_extract_intensity_ratio_features_globals;
	if( pro.n_inputs() < n_inputs_ )
	{
		std::cout << pro.name() << "dsm_extract_intensity_ratio_features: The input number should be " << n_inputs_ << std::endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	std::string filename = pro.get_input<std::string>(i++);

	dsm_features_sptr features_sptr = new dsm_features(filename);

	features_sptr->extract_intensity_ratio_features();

	pro.set_output_val(0,features_sptr);

	return true;
}//end dsm_extract_intensity_ratio_process