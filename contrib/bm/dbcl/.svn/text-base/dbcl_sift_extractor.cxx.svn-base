//this is /contrib/bm/dbcl/dbcl_sift_extractor.cxx
#include"dbcl_sift_extractor.h"

void dbcl_sift_extractor::extract_sift_features(vcl_map<unsigned, vil_image_resource_sptr> const& img_seq, 
													target_ncn_map_type& target_ncn_map,
														frame_target_kp_type& frame_target_kp_map,
															unsigned const& octave_size, unsigned const& num_octaves)
{
	//image iterators
	vcl_map<unsigned, vil_image_resource_sptr>::const_iterator img_itr, img_end = img_seq.end();

	//target iterators
	target_ncn_map_type::const_iterator target_itr, target_end = target_ncn_map.end();
	
	bapl_dense_sift_sptr dense_sift_sptr = new bapl_dense_sift();

	for( img_itr = img_seq.begin(); img_itr != img_end; ++img_itr )
	{
		dense_sift_sptr->create_pyramid(img_itr->second,octave_size,num_octaves);

		vcl_map<vgl_point_2d<unsigned>, vcl_vector<bapl_lowe_keypoint_sptr>,dbcl_vgl_point_2d_dist_compare > target_kp_map;

		for( target_itr = target_ncn_map.begin(); target_itr != target_end; ++target_itr )
		{
			bapl_lowe_keypoint_sptr kp_sptr = new bapl_lowe_keypoint();
			dense_sift_sptr->make_keypoint(kp_sptr,target_itr->first.x(),target_itr->first.y());

			vcl_vector<bapl_lowe_keypoint_sptr> kp_vector;

			dense_sift_sptr->make_keypoints(kp_vector, target_itr->second);

			kp_vector.insert(kp_vector.begin(),kp_sptr);
		
			target_kp_map[target_itr->first] = kp_vector;
		}//end target iteration

		frame_target_kp_map[img_itr->first] = target_kp_map;
	}//end image iteration
}//end dbcl_sift_extractor::extract_sift_features

void dbcl_sift_extractor::extract_sift_features(vcl_map<unsigned, vil_image_resource_sptr> const& img_seq, 
										target_ncn_map_type& target_ncn_map,
											frame_target_matrix_type& frame_target_matrix_map,
												unsigned const& octave_size, unsigned const& num_octaves )
{

	//image iterators
	vcl_map<unsigned, vil_image_resource_sptr>::const_iterator img_itr, img_end = img_seq.end();

	//target iterators
	target_ncn_map_type::const_iterator target_itr, target_end = target_ncn_map.end();

	//dense sift instance
	bapl_dense_sift_sptr dense_sift_sptr = new bapl_dense_sift();

	for( img_itr = img_seq.begin(); img_itr != img_end; ++img_itr )
	{
		dense_sift_sptr->create_pyramid(img_itr->second, octave_size, num_octaves);

		vcl_map<vgl_point_2d<unsigned>, vnl_matrix<double>, dbcl_vgl_point_2d_dist_compare> target_matrix_map;

		for( target_itr = target_ncn_map.begin(); target_itr != target_end; ++target_itr )
		{
			vcl_vector<bapl_lowe_keypoint_sptr> kp_vector;
			bapl_lowe_keypoint_sptr kp;

			dense_sift_sptr->make_keypoint(kp, target_itr->first.x(), target_itr->first.y());

			dense_sift_sptr->make_keypoints(kp_vector,target_itr->second);

			vnl_matrix<double> matrix(kp_vector.size()+1,128);

			matrix.set_row(0,kp->descriptor());

			for( unsigned r = 1; r <= target_itr->second.size(); ++r )
				matrix.set_row(r,kp_vector[r-1]->descriptor());

			target_matrix_map[target_itr->first] = matrix;
		}//end target iteration

		frame_target_matrix_map[img_itr->first] = target_matrix_map;

	}//end image iteration

}//end dbcl_sift_extractor::extract_sift_features