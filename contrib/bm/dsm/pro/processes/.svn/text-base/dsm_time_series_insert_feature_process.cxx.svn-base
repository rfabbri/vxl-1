//this is /contrib/bm/dsm/pro/processes/dsm_time_series_insert_feaeture_process.cxx
#include<bprb/bprb_func_process.h>
#include<bprb/bprb_parameters.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_time_series_sptr.h>

//:NOTE THIS PROCESS WILL ERASE THE FEATURE AT THE SPECIFIED TIME IF IT ALREADY EXISTS. 
//:
//:global variables
//: 3 inputs:
//:    1. dsm_time_series_sptr
//:    2. time
//:    3. dsm_feature_sptr
//: 0 outputs
namespace dsm_time_series_insert_feature_process_globals
{
	const unsigned int n_inputs_ = 3;
	const unsigned int n_outputs_ = 0;
}

bool dsm_time_series_insert_feature_process_cons(bprb_func_process& pro)
{
	//set input/output types
	using namespace dsm_time_series_insert_feature_process_globals;
	vcl_vector<vcl_string> input_types_(n_inputs_);
	unsigned i = 0;
	input_types_[i++] = "dsm_time_series_sptr";
	input_types_[i++] = "unsigned";
	input_types_[i++] = "dsm_feature_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

//the process
bool dsm_time_series_insert_feature_process(bprb_func_process& pro)
{
	using namespace dsm_time_series_insert_feature_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_time_series_insert_feature_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_time_series_sptr ts_sptr = pro.get_input<dsm_time_series_sptr>(i++);
	unsigned time = pro.get_input<unsigned>(i++);
	dsm_feature_sptr f_sptr = pro.get_input<dsm_feature_sptr>(i++);

	vcl_map<unsigned, dsm_feature_sptr>::iterator ts_itr, ts_end = ts_sptr->time_series.end();

	ts_itr = ts_sptr->time_series.find(time);

	if(ts_itr!=ts_end)
		ts_sptr->time_series.erase(ts_itr);

	ts_sptr->time_series[time] = f_sptr;

	return true;
}