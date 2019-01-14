//this is /contrib/bm/dsm/pro/processes/dsm_ground_truth_read_bwm_bin_process.cxx
#include<bprb/bprb_func_process.h>

#include<dsm/dsm_utilities.h>

#include<iostream>

#include<vidl/vidl_image_list_istream.h>
#include<vidl/vidl_convert.h>

#include<vil/vil_image_view.h>
#include<vil/vil_load.h>

#include<vnl/vnl_matrix.h>

namespace dsm_utility_compute_confusion_matrix_from_change_maps_process_globals
{
	const unsigned int n_inputs_ = 4;
	const unsigned int n_outputs_ = 0;
}

bool dsm_utility_compute_confusion_matrix_from_change_maps_process_cons(bprb_func_process& pro)
{
	using namespace dsm_utility_compute_confusion_matrix_from_change_maps_process_globals;

	std::vector<std::string> input_types_(n_inputs_);
	std::vector<std::string> output_types_(n_outputs_);

	unsigned i = 0;
	input_types_[i++] = vcl_string";//ground truth image file glob
	input_types_[i++] = vcl_string";//predicted image file glob
	input_types_[i++] = vcl_string";//result filename
	input_types_[i++] = "unsigned";//frame start bias

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dsm_utility_compute_confusion_matrix_from_change_maps_process(bprb_func_process& pro)
{
	using namespace dsm_utility_compute_confusion_matrix_from_change_maps_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		std::cerr << pro.name() << " dsm_utility_compute_confusion_matrix_from_change_maps_process: The input number should be " << n_inputs_ << std::flush;
		return false;
	}

	//get inputs
	unsigned i = 0;
	std::string gt_file_glob = pro.get_input<std::string>(i++);
	std::string predicted_file_glob = pro.get_input<std::string>(i++);
	std::string filename = pro.get_input<std::string>(i++);
	unsigned bias = pro.get_input<unsigned>(i++);

	vidl_image_list_istream gt_video_stream, predicted_video_stream;

	if(!gt_video_stream.open(gt_file_glob))
	{
		std::cerr << "----ERROR---- dsm_utility_compute_confusion_matrix_from_change_maps_process"
			     << "\tGround truth video stream failed to open.\n" << std::flush;
		return false;
	}

	if(!predicted_video_stream.open(predicted_file_glob))
	{
		std::cerr << "----ERROR---- dsm_utility_compute_confusion_matrix_from_change_maps_process"
			     << "\tPredicted video stream failed to open.\n" << std::flush;
		return false;
	}

	if( gt_video_stream.num_frames() != predicted_video_stream.num_frames() )
	{
		std::cerr << "----ERROR---- dsm_utility_compute_confusion_matrix_from_change_maps_process"
			     << "\tNumber of frames for ground truth and predicted images do not match.\n" << std::flush;
		return false;
	}

	if( gt_video_stream.width() != predicted_video_stream.width() )
	{
		std::cerr << "----ERROR---- dsm_utility_compute_confusion_matrix_from_change_maps_process"
			     << "\tGround truth and predicted frame width do not match.\n" << std::flush;
		return false;
	}

	if( gt_video_stream.height() != predicted_video_stream.height() )
	{
		std::cerr << "----ERROR---- dsm_utility_compute_confusion_matrix_from_change_maps_process"
			     << "\tGround truth and predicted frame height do not match.\n" << std::flush;
		return false;
	}

	vnl_matrix<double> confusion_matrix(2,2,double(0.0));

	for(unsigned i = 0 + bias; i < gt_video_stream.num_frames(); ++i)
	{
		std::cout << "Processing Frame: " << i+1 << " out of " << gt_video_stream.num_frames() << std::endl;
		gt_video_stream.seek_frame(i);
		predicted_video_stream.seek_frame(i);

		vil_image_view<vxl_byte> gt_view, prediction_view;

		vidl_convert_to_view(*gt_video_stream.current_frame(),gt_view);
		vidl_convert_to_view(*predicted_video_stream.current_frame(), prediction_view);

		confusion_matrix+=dsm_utilities::compute_confusion_matrix(gt_view,prediction_view);
	}

	std::ofstream of(filename.c_str(), std::ios::out);

	of <<  confusion_matrix[0][0] << '\t' << confusion_matrix[0][1] << '\n';;
	
	of <<  confusion_matrix[1][0] << '\t' << confusion_matrix[1][1] << '\n';
	
	//: confusion_matrix[0][0] = true positive
	//: confusion_matrix[0][1] = false negative
	//: confusion_matrix[1][0] = false positive
	//: confusion_matrix[1][1] = true negative

	
	////true positive rate (sensitivity) = tp/(tp+fn);
	//of << confusion_matrix[0][0]/(confusion_matrix[0][0] + confusion_matrix[0][1])
	//   << '\n';

	////false positive rate = fp/(fp+tn)
	//of << confusion_matrix[1][0]/(confusion_matrix[1][0] + confusion_matrix[1][1])
	//   << '\n';

	of.close();
	
	return true;
}