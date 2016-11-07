//this is /contrib/bm/dsm/pro/processes/dsm_ground_truth_write_change_maps_process.cxx
#include<bprb/bprb_func_process.h>

#include<dsm/dsm_ground_truth_sptr.h>
#include<dsm/io/dsm_io_ground_truth.h>

#include<vcl_iomanip.h>
#include<vcl_iostream.h>
#include<vcl_string.h>
#include<vcl_sstream.h>
#include<vcl_vector.h>

#include<vul/vul_file.h>

namespace dsm_ground_truth_write_change_maps_process_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_ground_truth_write_change_maps_process_cons( bprb_func_process& pro )
{
	using namespace dsm_ground_truth_write_change_maps_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_ground_truth_sptr";
	input_types_[i++] = "vcl_string";//output directory

	if(!pro.set_input_types(input_types_))
		return false;
	
	return true;
}

bool dsm_ground_truth_write_change_maps_process( bprb_func_process& pro )
{
	using namespace dsm_ground_truth_write_change_maps_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_ground_truth_write_change_maps_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_ground_truth_sptr gt_sptr = pro.get_input<dsm_ground_truth_sptr>(i++);
	vcl_string result_dir = pro.get_input<vcl_string>(i++);

	gt_sptr->save_change_maps_tiff(result_dir);

	return true;
}