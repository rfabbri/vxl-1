//this is /contrib/bm/dsm/dsm_features.h
#ifndef DSM_FEATURES_H_
#define DSM_FEATURES_H_
//:
// \file
// \date January 12, 2011
// \author Brandon A. Mayer
//
// Given an xml file containing the target and neighborhood information as well as a video glob,
// extract features of the form (frame, target, feature vector).
//
// \verbatim
//  Modifications
// \endverbatim
#include<bapl/bapl_dense_sift_sptr.h>
#include<bapl/bapl_dsift_sptr.h>

#include<bxml/bxml_document.h>
#include<bxml/bxml_read.h>
#include<bxml/bxml_write.h>

#include"dsm_utilities.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_map.h>
#include<vcl_string.h>
#include<vcl_vector.h>

#include<vgl/vgl_point_2d.h>
#include<vgl/io/vgl_io_point_2d.h>

#include<vidl/vidl_image_list_istream.h>
#include<vidl/vidl_convert.h>

#include<vil/vil_image_resource_sptr.h>
#include<vil/vil_image_view.h>
#include<vil/vil_convert.h>
#include<vil/vil_new.h>

#include<vnl/vnl_math.h> //for vnl_eps
#include<vnl/vnl_matrix.h>
#include<vnl/algo/vnl_svd.h>
#include<vnl/io/vnl_io_vector.h>

#include<vsl/vsl_binary_io.h>

class dsm_features: public vbl_ref_count
{
public:


	dsm_features(){}

	dsm_features(vcl_string const& neighborhood_xml_path);

	dsm_features(vcl_string const& video_glob,
					vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned> > const& neighborhood);

	vcl_string feature_type();

	vcl_map<vgl_point_2d<unsigned>, vcl_map<unsigned, vnl_vector<double> >, dsm_vgl_point_2d_coord_compare<unsigned>  > feature_map()
	{return this->feature_map_;}

	vcl_map<vgl_point_2d<unsigned>, vcl_map<unsigned, vnl_vector<double> >, dsm_vgl_point_2d_coord_compare<unsigned>  > reduced_feature_map()
	{ return this->reduced_feature_map_; }

	vcl_map<vgl_point_2d<unsigned>, vcl_map<unsigned, vnl_vector<double> >, dsm_vgl_point_2d_coord_compare<unsigned> >::iterator reduced_feature_map_begin()
	{return this->reduced_feature_map_.begin();}

	vcl_map<vgl_point_2d<unsigned>, vcl_map<unsigned, vnl_vector<double> >, dsm_vgl_point_2d_coord_compare<unsigned> >::iterator reduced_feature_map_end()
	{return this->reduced_feature_map_.end();}

	vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned> > neighborhood()
	{return this->neighborhood_;}

	bool parse_neighborhood_xml( vcl_string const& neighborhood_xml_path);

	bool extract_sift_features(unsigned const& octave_size = 6, unsigned const& num_octaves = 1);

	bool extract_dsift_features(float const& orientation = 0.0f);

	bool extract_intensity_ratio_features();

	bool extract_intensity_features();

	bool extract_hog_features(unsigned const& block_size = 16, unsigned const& cell_size = 4);

	bool reduce_dimensionality(unsigned const& ndims_to_keep);

	void write_features_xml( vcl_string const& filename );

	bool write_reduced_features_mfile( vcl_string const& filename ) const;

	void b_write(vsl_b_ostream& os) const;

	void b_read(vsl_b_istream& is);

private:


	vcl_string neighborhood_xml_path_;
	vcl_string video_glob_;
	
	unsigned octave_size_;
	unsigned num_octaves_;
	unsigned num_targets_;
	unsigned num_neighbors_;
	bool neighborhood_valid_;
	bool features_valid_;
	bool dim_reduced_;

	//target:frame:feature map
	//because each target may have more than one feature vector associated with it at a given time we will store this in matrix form
	//the rows indicating the ith feature vector, the columns the value of the jth dimension of the feature vector
	vcl_map<vgl_point_2d<unsigned>, vcl_map<unsigned, vnl_vector<double> >, dsm_vgl_point_2d_coord_compare<unsigned>  > feature_map_;
	vcl_map<vgl_point_2d<unsigned>, vcl_map<unsigned, vnl_vector<double> >, dsm_vgl_point_2d_coord_compare<unsigned>  > reduced_feature_map_;
	vcl_map<vgl_point_2d<unsigned>, vcl_map<unsigned, bapl_lowe_keypoint_sptr >, dsm_vgl_point_2d_coord_compare<unsigned>  > lowe_keypoint_map_;
	vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned> > neighborhood_;
};

#endif //DSM_FEATURES_H_