//this is /contrib/bm/dsm/pro/processes/dsm_extract_intensity_feature_process.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include"dsm/dsm_feature_sptr.h"
#include"dsm/dsm_target_neighborhood_map_sptr.h"

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

namespace dsm_extract_intensity_ratio_feature_process_globals
{
	const unsigned int n_inputs_ = 5;
	const unsigned int n_outputs_ = 1;
}

bool dsm_extract_intensity_ratio_feature_process_cons(bprb_func_process& pro)
{
	using namespace dsm_extract_intensity_ratio_feature_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_target_neighborhood_map_sptr";
	input_types_[i++] = "unsigned";//target x coordinate
	input_types_[i++] = "unsigned";//target y coordinate
	input_types_[i++] = "unsigned";//time
	input_types_[i++] = "vil_image_view_base_sptr";
	

	output_types_[0] = "dsm_feature_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}

bool dsm_extract_intensity_ratio_feature_process(bprb_func_process& pro)
{
	using namespace dsm_extract_intensity_ratio_feature_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_extract_intensity_ratio_feature_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}	

	//get inputs
	unsigned i = 0;
	dsm_target_neighborhood_map_sptr target_neighborhood_map_sptr = pro.get_input<dsm_target_neighborhood_map_sptr>(i++);
	unsigned target_x = pro.get_input<unsigned>(i++);
	unsigned target_y = pro.get_input<unsigned>(i++);
	unsigned time = pro.get_input<unsigned>(i++);
	vil_image_view_base_sptr vil_base_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
	
	if( vil_base_sptr->nplanes() > 1 || vil_base_sptr->pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE )
	{
		vcl_cout << "---- ERROR: dsm_extract_intensity_ratio_feature_process ----\n"
			     << "\t Please provide a grey scale image. " << vcl_endl;
		return false;
	}

	dsm_target_neighborhood_map::dsm_target_neighborhood_map_type::const_iterator 
		t_itr=target_neighborhood_map_sptr->target_neighborhood_map_.find(vgl_point_2d<unsigned>(target_x,target_y));

	if( t_itr == target_neighborhood_map_sptr->target_neighborhood_map_.end() )
	{
		vcl_cout << "---- ERROR: dsm_extract_intensity_ratio_feature_process ----\n"
			<< "\t Target (" << target_x << "," << target_y << ") not found in the target/neighborhood map. " << vcl_endl;
		return false;
	}

	vil_image_view<vxl_byte> img_view(vil_base_sptr);

	vnl_vector<double> intensity_ratio(t_itr->second.size());

	vcl_vector<vgl_point_2d<unsigned> >::const_iterator nitr, nend = t_itr->second.end();

	double target_intensity = img_view(t_itr->first.x(), t_itr->first.y());

	unsigned indx;
	for( indx = 0, nitr = t_itr->second.begin(); nitr != nend; ++nitr, ++indx )
		intensity_ratio[indx] = target_intensity/img_view(nitr->x(),nitr->y());
	
	dsm_feature_sptr feature_sptr = new dsm_feature(intensity_ratio,time);

	pro.set_output_val(0,feature_sptr);

	return true;
}