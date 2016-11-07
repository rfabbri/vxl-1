//this is /contrib/bm/dsm/pro/processes/dsm_create_time_series_process.cxx
#include<bprb/bprb_func_process.h>
#include<bprb/bprb_parameters.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_time_series_sptr.h>

//:global variables
//:no inputs
//:outputs new time_series_sptr to the database
namespace dsm_create_time_series_process_globals
{
	const unsigned int n_inputs_ = 0;
	const unsigned int n_outputs_ = 1;
}

bool dsm_create_time_series_process_cons(bprb_func_process& pro)
{
	using namespace dsm_create_time_series_process_globals;

	vcl_vector<vcl_string> output_types_(n_outputs_);
	output_types_[0] = "dsm_time_series_sptr";

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}//end process cons

//:creates the dsm_time_series_sptr to be commited to db
bool dsm_create_time_series_process(bprb_func_process& pro)
{
	using namespace dsm_create_time_series_process_globals;

	dsm_time_series_sptr time_series_sptr = new dsm_time_series();

	pro.set_output_val(0, time_series_sptr);

	return true;
}//end process