//this is /contrib/bm/dsm/pro/processes/dsm_ncn_add_target_process.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_ncn_sptr.h>

//:global variables
namespace dsm_ncn_add_target_process_globals
{
	const unsigned int n_inputs_ = 3;
	const unsigned int n_outputs_ = 0;
}

bool dsm_ncn_add_target_process_cons(bprb_func_process& pro)
{
	using namespace dsm_ncn_add_target_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_ncn_sptr";
	input_types_[i++] = "unsigned";//target x coordinate
	input_types_[i++] = "unsigned";//target y coordinate

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dsm_ncn_add_target_process(bprb_func_process& pro)
{
	using namespace dsm_ncn_add_target_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_ncn_add_target_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_ncn_sptr ncn_sptr = pro.get_input<dsm_ncn_sptr>(i++);
	unsigned key_x = pro.get_input<unsigned>(i++);
	unsigned key_y = pro.get_input<unsigned>(i++);

	ncn_sptr->add_target(vgl_point_2d<unsigned>(key_x,key_y));

	return true;
}

