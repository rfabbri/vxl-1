//this is /contrib/bm/dsm/pro/processes/dsm_ground_truth_read_bin_process.cxx
#include<bprb/bprb_func_process.h>

#include<dsm/dsm_ground_truth_sptr.h>
#include<dsm/io/dsm_io_ground_truth.h>

#include<vcl_iostream.h>
#include<vcl_string.h>
#include<vcl_vector.h>

namespace dsm_ground_truth_read_bin_process_globals
{
	const unsigned int n_inputs_ = 1;
	const unsigned int n_outputs_ = 1;
}

bool dsm_ground_truth_read_bin_process_cons( bprb_func_process& pro )
{
	using namespace dsm_ground_truth_read_bin_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	input_types_[0] = "vcl_string";//filename
	output_types_[0] = "dsm_ground_truth_sptr";

	if( !pro.set_input_types(input_types_))
		return false;

	if( !pro.set_output_types(output_types_))
		return false;

	return true;
}

bool dsm_ground_truth_read_bin_process(bprb_func_process& pro)
{
	//using namespace dsm_ground_truth_read_bin_process_globals;

	//if( pro.n_inputs() < n_inputs_ )
	//{
	//	vcl_cout << pro.name() << " dsm_ground_truth_read_bin_process: The input number should be " << n_inputs_ << vcl_endl;
	//	return false;
	//}

	////get inputs
	//vcl_string filename = pro.get_input<vcl_string>(0);

	//vsl_b_ifstream is(filename.c_str(), vcl_ios::in|vcl_ios::binary);

	//dsm_ground_truth* gt_ptr = new dsm_ground_truth();

	//vsl_b_read(is, gt_ptr);

	//dsm_ground_truth_sptr gt_sptr(gt_ptr);

	//pro.set_output_val(0,gt_ptr);

	return true;
}