//this is /contrib/bm/vlfeat/vlfeat_dsift.cxx
#include"vlfeat.h"

vcl_map<vgl_point_2d<float>, vcl_vector<float>,vlfeat_vgl_point_2d_coord_compare<float> > vlfeat::dsift( vil_image_view<float> &img,
																									 unsigned const& step,
																								     unsigned const& size,
																								     bool const& flat_window )
{
	vcl_map<vgl_point_2d<float>, vcl_vector<float>,vlfeat_vgl_point_2d_coord_compare<float> > target_descriptor_map;

	vl_bool floatDescriptors = VL_TRUE;
	vl_bool useFlatWindow = flat_window ? VL_TRUE : VL_FALSE;

	VlDsiftFilter* dsiftFilter = vl_dsift_new_basic(img.ni(),img.nj(),step,size);

	vl_dsift_set_bounds(dsiftFilter, 0, 0, img.ni(), img.nj());
	vl_dsift_set_flat_window(dsiftFilter, useFlatWindow);

	float* data = img.top_left_ptr();

#if 1
	{
	int stepX;
	int stepY;
	int minX;
	int minY;
	int maxX;
	int maxY;
	vl_bool useFlatWindowTest;

	vl_dsift_get_steps(dsiftFilter, &stepY, &stepX);
	vl_dsift_get_bounds(dsiftFilter, &minY, &minX, &maxY, &maxX);

	vcl_cout << "stepX = " << stepX << vcl_endl;
	vcl_cout << "stepY = " << stepY << vcl_endl;
	vcl_cout << "minX = " << minX << vcl_endl;
	vcl_cout << "minY = " << minY << vcl_endl;
	vcl_cout << "maxX = " << maxX << vcl_endl;
	vcl_cout << "maxY = " << maxY << vcl_endl;
	}

#endif
	int numFrames = vl_dsift_get_keypoint_num(dsiftFilter);
	int descrSize = vl_dsift_get_descriptor_size(dsiftFilter);

	assert(numFrames<target_descriptor_map.max_size());
	
	//run the dense sift
	vl_dsift_process(dsiftFilter, data);

	//int numFrames = vl_dsfit_get_keypoint_num(dsiftFilter);
	//int descrSize = vl_dsfit_get_descriptor_size(dsiftFilter);
	VlDsiftKeypoint const *frames;
	//VlDsiftDescriptorGeometry const* geom;
	float const *descrs;
	frames = vl_dsift_get_keypoints(dsiftFilter);
	descrs = vl_dsift_get_descriptors(dsiftFilter);

	//int numFrames = vl_dsift_get_keypoint_num(dsiftFilter);
	//int descrSize = vl_dsift_get_descriptor_size(dsiftFilter);

	for(int i = 0; i < numFrames; ++i)
	{
		vgl_point_2d<float> pt(frames[i].x, frames[i].y);
		vcl_vector<float> descriptor(descrSize,float(0));
		
		for(int j = 0; j < descrSize; ++j)
		{
			descriptor[j] = (float)descrs[i*descrSize+j];
			descriptor[j] = VL_MIN(512.0F*descriptor[j],255.0F);
		}

		//vcl_cout << pt << vcl_endl;
		target_descriptor_map[pt] = descriptor;
		//vcl_pair<vcl_map<vgl_point_2d<unsigned>,vcl_vector<float>,vlfeat_vgl_point_2d_coord_compare>::iterator,bool> ret;
		//ret=target_descriptor_map.insert(vcl_pair<vgl_point_2d<unsigned>,vcl_vector<float> >(pt, descriptor));
		//if(ret.second == false)
		//	vcl_cout << "pt: " << pt << " already exists." << vcl_endl;
	}
	
	//deleting the dsift filter object.
	vl_dsift_delete(dsiftFilter);
	return target_descriptor_map;
}//end vlfeat_dsift
