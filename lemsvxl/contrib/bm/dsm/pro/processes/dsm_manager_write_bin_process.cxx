//this is /contrib/bm/dsm/pro/processes/dsm_manager_write_bin_process.cxx
#include<bprb/bprb_func_process.h>

#include<dsm/dsm_manager_base_sptr.h>
#include<dsm/dsm_manager.h>
#include<dsm/io/dsm_io_manager.h>

#include<iostream>

namespace dsm_manager_write_bin_process_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_manager_write_bin_process_cons( bprb_func_process& pro )
{
	using namespace dsm_manager_write_bin_process_globals;

	std::vector<std::string> input_types_(n_inputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_manager_base_sptr";
	input_types_[i++] = vcl_string";//output filename

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dsm_manager_write_bin_process( bprb_func_process& pro )
{
	using namespace dsm_manager_write_bin_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		std::cout << pro.name() << " dsm_manager_write_bin_process: The input number should be " << n_inputs_ << std::endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_manager_base_sptr manager_sptr = pro.get_input<dsm_manager_base_sptr>(i++);
	std::string filename = pro.get_input<std::string>(i++);

	vsl_b_ofstream of(filename.c_str(), std::ios::out|std::ios::binary);

	dsm_manager<2>* manager_ptr = static_cast<dsm_manager<2>*>(manager_sptr.as_pointer());

	vsl_b_write(of,manager_ptr);

	return true;
}