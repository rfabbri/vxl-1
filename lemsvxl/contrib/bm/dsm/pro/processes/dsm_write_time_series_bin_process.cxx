//this is /contrib/bm/dsm/pro/processes/dsm_write_time_series_bin_process.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include"dsm/dsm_time_series_sptr.h"

namespace dsm_write_time_series_bin_process_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_write_time_series_bin_process_cons( bprb_func_process& pro )
{
	using namespace dsm_write_time_series_bin_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);

	unsigned i = 0;
	input_types_[i++] = "vcl_string"; //filename
	input_types_[i++] = "dsm_time_series_sptr";//the sptr to write

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dsm_write_time_series_bin_process( bprb_func_process& pro )
{
	using namespace dsm_write_time_series_bin_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_write_time_series_bin_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get input
	unsigned i = 0;

	vcl_string filename = pro.get_input<vcl_string>(i++);

	dsm_time_series_sptr p = pro.get_input<dsm_time_series_sptr>(i++);

	//create output stream
	vsl_b_ofstream os(filename.c_str(), vcl_ios::out|vcl_ios::binary);

	if(!os)
		return false;

	vsl_b_write(os,p);

	os.close();

	return true;
}