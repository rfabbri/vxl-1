//this is /contrib/bm/dbcl/dbcl_sift_extractor.h
#ifndef DBCL_SIFT_EXTRACTOR_H_
#define DBCL_SIFT_EXTRACTOR_H_

//:
// \file
// \date January 3, 2011
// \author Brandon A. Mayer
//
// This class takes a list of targets and neighborhoods and extracts sift features at each point.
//		Input: vcl_map< vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> > 
//			This is a map between a target location and a neighborhood
//
//		Ouput:vcl_map<vgl_point_2d<unsigned>, vnl_matrix<double> > 
//			This is a map between a target and the concatinated feature vectors. The first row is the feature vector for the target, the
//			rest correspond to features of the neighbors.
//		OR vcl_map<vgl_point_2d<unsigned>,vcl_vector<bapl_lowe_keypoint_sptr> >
//			This is a map between a target and the raw lowe keypoints.
// \verbatim
//  Modifications
// \endverbatim

#include"bapl/bapl_dense_sift_sptr.h"

#include"dbcl_ncn_extractor.h"
#include"dbcl_vgl_point_2d_dist_compare.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_map.h>
#include<vcl_utility.h>

#include<vgl/vgl_point_2d.h>

#include<vil/vil_image_resource_sptr.h>

class dbcl_sift_extractor
{
public:
	//maps frames to targets to feature matrices
	typedef vcl_map<unsigned, vcl_map<vgl_point_2d<unsigned>, vnl_matrix<double>, dbcl_vgl_point_2d_dist_compare > > frame_target_matrix_type;

	//maps frames to targets to lowe keypoints
	typedef vcl_map<unsigned, vcl_map<vgl_point_2d<unsigned>, vcl_vector<bapl_lowe_keypoint_sptr>, dbcl_vgl_point_2d_dist_compare > > frame_target_kp_type;

	typedef dbcl_ncn_extractor::target_ncn_map_type target_ncn_map_type; //this will be the input

	//dbcl_sift_extractor():dense_sift_sptr(NULL){}

	static void extract_sift_features(vcl_map<unsigned, vil_image_resource_sptr> const& img_seq, 
										target_ncn_map_type& target_ncn_map,
											frame_target_kp_type& frame_target_kp_map,
												unsigned const& octave_size = 6, unsigned const& num_octaves = 1);

	static void extract_sift_features(vcl_map<unsigned, vil_image_resource_sptr> const& img_seq, 
										target_ncn_map_type& target_ncn_map,
											frame_target_matrix_type& frame_target_matrix_map,
												unsigned const& octave_size = 6, unsigned const& num_octaves = 1);
		

	~dbcl_sift_extractor(){}
											
protected:
	dbcl_sift_extractor(){}
//private:
//	vcl_map<unsigned, bapl_dense_sift_sptr> frame_dense_sift_map;
};

#endif //DBCL_SIFT_EXTRACTOR_H_