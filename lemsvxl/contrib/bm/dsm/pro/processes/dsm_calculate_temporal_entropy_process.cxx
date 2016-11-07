//this is /contrib/bm/dsm/pro/processes/dsm_calculate_temporal_entropy_process.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_ncn_sptr.h>

#include<vcl_iostream.h>
#include<vcl_vector.h>
#include<vcl_string.h>

//:global variables
namespace dsm_calculate_temporal_entropy_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 1;
}

//:sets input and output types
bool dsm_calculate_temporal_entropy_process_cons(bprb_func_process& pro)
{
	//set input/output types
	using namespace dsm_calculate_temporal_entropy_globals;

	//takes in 2 inputs, the video stream glob and number of histogram bins.
	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);
	input_types_[0] = "vcl_string";
	input_types_[1] = "unsigned";
	output_types_[0]="dsm_ncn_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}//end dsm_calculate_temporal_entropy_process_cons

//:calculates temporal entropy
bool dsm_calculate_temporal_entropy_process(bprb_func_process& pro)
{
	using namespace dsm_calculate_temporal_entropy_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_calculate_temporal_entropy_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get intputs
	unsigned i = 0;
	vcl_string video_glob = pro.get_input<vcl_string>(i++);
	unsigned nbins = pro.get_input<unsigned>(i++);

	dsm_ncn_sptr ncn_sptr = new dsm_ncn();

	ncn_sptr->load_video(video_glob);
	ncn_sptr->calculate_temporal_entropy(nbins);

	pro.set_output_val(0,ncn_sptr);

	return true;
}//end dsm_calculate_temporal_entropy_process