//this is /contrib/bm/dsm/pro/processes/dsm_utility_bvxm_binary_to_byte_process.cxx
#include<bprb/bprb_func_process.h>

#include<vidl/vidl_image_list_istream.h>
#include<vidl/vidl_convert.h>

#include<vcl_iomanip.h>
#include<vcl_string.h>
#include<vcl_sstream.h>

#include<vil/vil_image_view.h>
#include<vil/vil_load.h>
#include<vil/vil_save.h>

#include<vul/vul_file.h>

namespace dsm_utility_bvxm_binary_to_byte_process_globals
{
	const unsigned int n_inputs_ = 6;
	const unsigned int n_outputs_ = 0;
}

bool dsm_utility_bvxm_binary_to_byte_process_cons( bprb_func_process& pro )
{
	using namespace dsm_utility_bvxm_binary_to_byte_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	
	unsigned i = 0;
	input_types_[i++] = "vcl_string";//bvxm float images
	input_types_[i++] = "vcl_string";//byte output directory
	
	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dsm_utility_bvxm_binary_to_byte_process( bprb_func_process& pro )
{
	using namespace dsm_utility_bvxm_binary_to_byte_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cerr << pro.name() << " dsm_utility_bvxm_binary_to_byte_process: The input number should be " << n_inputs_ << vcl_flush;
		return false;
	}

	//get inputs
	unsigned i = 0;
	vcl_string bvxm_file_glob = pro.get_input<vcl_string>(i++);
	vcl_string byte_file_dir = pro.get_input<vcl_string>(i++);

	if(!vul_file::is_directory(byte_file_dir))
		vul_file::make_directory(byte_file_dir);

	vidl_image_list_istream bvxm_video_stream;

	if(!bvxm_video_stream.open(bvxm_file_glob))
	{
		vcl_cerr << "----ERROR---- dsm_utility_bvxm_binary_to_byte_process_globals"
			     << "\tbvxm float video stream failed to open.\n" << vcl_flush;
		return false;
	}

	unsigned nframes = bvxm_video_stream.num_frames();
	unsigned ni = bvxm_video_stream.width();
	unsigned nj = bvxm_video_stream.height();

	for( unsigned frame = 0; frame < nframes; ++frame )
	{
		vcl_cout << "\tConverting frame " << frame << " out of " << nframes << vcl_endl;
		bvxm_video_stream.seek_frame(frame);

		vil_image_view<bool> bvxm_view;
		vidl_convert_to_view(*bvxm_video_stream.current_frame(), bvxm_view);

		vil_image_view<vxl_byte> byte_view(ni,nj,1);

		for(unsigned i = 0; i < ni; ++i)
			for(unsigned j = 0; j < nj; ++j)
				byte_view(i,j,0) = vxl_byte(255)*bvxm_view(i,j,0);

		vcl_stringstream filename;

		filename << byte_file_dir << vcl_setfill('0') << vcl_setw(8) << "/byte_change_" <<  frame << ".tiff";

		vil_save(byte_view,filename.str().c_str());
	}

	return true;
}