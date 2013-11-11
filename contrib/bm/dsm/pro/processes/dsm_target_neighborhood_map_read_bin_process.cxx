//this is /contrib/bm/dsm/pro/processes/dsm_target_neighborhood_map_read_bin_process.cxx
#include<bprb/bprb_func_process.h>
#include<bprb/bprb_parameters.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_target_neighborhood_map_sptr.h>

#include<vbl/io/vbl_io_smart_ptr.h>

namespace dsm_target_neighborhood_map_read_bin_process_globals
{
	const unsigned int n_inputs_ = 1;
	const unsigned int n_outputs_ = 1;
}

bool dsm_target_neighborhood_map_read_bin_process_cons( bprb_func_process& pro )
{
	//set input/output types
	using namespace dsm_target_neighborhood_map_read_bin_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	input_types_[0] = "vcl_string";//the filename to read 
	output_types_[0] = "dsm_target_neighborhood_map_sptr";//output a dsm_target_neighborhood_map_sptr

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}

bool dsm_target_neighborhood_map_read_bin_process( bprb_func_process& pro )
{
	using namespace dsm_target_neighborhood_map_read_bin_process_globals;

	if( pro.n_inputs() != n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_target_neighborhood_map_write_bin_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get input
	vcl_string filename = pro.get_input<vcl_string>(0);

	dsm_target_neighborhood_map_sptr target_neighborhood_map_sptr = new dsm_target_neighborhood_map();

	vsl_b_ifstream is(filename.c_str(), vcl_ios::in|vcl_ios::binary);

	vsl_b_read(is,target_neighborhood_map_sptr);

	is.close();

	return true;

}