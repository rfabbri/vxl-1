//this is /contrib/bm/dsm/pro/processes/dsm_write_reduced_features_mfile.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_features_sptr.h>

#include<vcl_iostream.h>
#include<vcl_vector.h>
#include<vcl_string.h>

//INPUTS: 1. output filename
//		  2. dsm_features_sptr with the features to be written
//OUTPUTS: NONE

//:global variables
namespace dsm_write_reduced_features_mfile_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_write_reduced_features_mfile_process_cons( bprb_func_process& pro )
{
	//set input/output types
	using namespace dsm_write_reduced_features_mfile_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	
	input_types_[0] = "vcl_string";
	input_types_[1]=  "dsm_features_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}//end dsm_write_reduced_features_mfile_process_cons

bool dsm_write_reduced_features_mfile_process(bprb_func_process& pro)
{
	using namespace dsm_write_reduced_features_mfile_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_extract_sift_features: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get intputs
	unsigned  i = 0;
	vcl_string filename = pro.get_input<vcl_string>(i++);
	dsm_features_sptr features_sptr = pro.get_input<dsm_features_sptr>(i++);

	features_sptr->write_reduced_features_mfile(filename);

	return true;
}//end dsm_write_reduced_features_mfile_process