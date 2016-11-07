//this is /contrib/bm/dsm/pro/processes/dsm_write_time_series_dat_process.cxx
#include<bprb/bprb_func_process.h>
#include<bprb/bprb_parameters.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_time_series_sptr.h>

//:global variables
//:2 inputs:
//:		1. dsm_time_series_sptr
//:		2. dat filename
//:0 outputs
namespace dsm_write_time_series_dat_process_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_write_time_series_dat_process_cons(bprb_func_process& pro)
{
	//set input/output types
	using namespace dsm_write_time_series_dat_process_globals;
	vcl_vector<vcl_string> input_types_(n_inputs_);
	unsigned i = 0;
	input_types_[i++] = "dsm_time_series_sptr";
	input_types_[i++] = "vcl_string";

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dsm_write_time_series_dat_process(bprb_func_process& pro)
{
	using namespace dsm_write_time_series_dat_process_globals; 

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_write_time_series_dat_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_time_series_sptr ts_sptr = pro.get_input<dsm_time_series_sptr>(i++);
	vcl_string filename = pro.get_input<vcl_string>(i++);

	return ts_sptr->write_txt(filename);
}