//this is /contrib/bm/dsm/pro/processes/dsm_ncn_set_video_glob_process.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_ncn_sptr.h>

//:global variables
namespace dsm_ncn_set_video_glob_process_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_ncn_set_video_glob_process_cons(bprb_func_process& pro)
{
	using namespace dsm_ncn_set_video_glob_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_ncn_sptr";
	input_types_[i++] = "vcl_string";

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dsm_ncn_set_video_glob_process(bprb_func_process& pro)
{
	using namespace dsm_ncn_set_video_glob_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_ncn_set_video_glob_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_ncn_sptr ncn_sptr = pro.get_input<dsm_ncn_sptr>(i++);
	vcl_string video_glob = pro.get_input<vcl_string>(i++);

	ncn_sptr->set_video_glob(video_glob);

	return true;
}