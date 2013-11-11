//this is /contrib/bm/dsm/pro/processes/dsm_reduced_features_dimensionality.cxx
//:
// \file
// \date February 10, 2011
// \author Brandon A. Mayer
//
// Process to reduce dimensionality of features in dsm_features
//
// Inputs: 1. dsm_featuse_sptr
//		   2. number of dimensions to keep (unsigned)
//
// \verbatim
//  Modifications
// \endverbatim
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_features_sptr.h>

#include<vcl_iostream.h>
#include<vcl_vector.h>
#include<vcl_string.h>

//:global variables
namespace dsm_reduce_features_dimensionality_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_reduce_features_dimensionality_process_cons(bprb_func_process& pro)
{
	//set input/output types
	using namespace dsm_reduce_features_dimensionality_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	input_types_[0] = "dsm_features_sptr";
	input_types_[1] = "unsigned";

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}//end dsm_reduce_feature_dimensionality_process_cons

bool dsm_reduce_features_dimensionality_process(bprb_func_process& pro)
{
	using namespace dsm_reduce_features_dimensionality_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_reduce_feature_dimensionality_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_features_sptr features_sptr = pro.get_input<dsm_features_sptr>(i++);
	unsigned ndims_to_keep = pro.get_input<unsigned>(i++);

	return features_sptr->reduce_dimensionality(ndims_to_keep);
}//end dsm_reduce_feature_dimensionality_process