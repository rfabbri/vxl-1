//this is contrib/bm/ncn1/ncn1_feature.h
#ifndef ncn1_feature_h_
#define ncn1_feature_h_

#include"ncn1_vgl_point_2d_less_than.h"

#include<vbl/vbl_ref_count.h>
//#include<vbl/vbl_array_1d.h>

#include<vcl_map.h>
#include<vcl_string.h>
#include<vcl_vector.h>

#include<vgl/vgl_point_2d.h>
#include<vgl/vgl_point_3d.h>

#include<vil/vil_image_view.h>

#include<vnl/algo/vnl_svd.h>

#include<vnl/vnl_matrix.h>
#include<vnl/vnl_vector.h>
#include<vnl/vnl_vector_fixed.h>


namespace feature_typedefs
{
    typedef vnl_vector<vxl_byte> intensity_vector_type;

    typedef vcl_map< vgl_point_2d<unsigned>, intensity_vector_type, ncn1_vgl_point_2d_less_than > feature_vector_type;

    typedef vcl_map< unsigned, feature_vector_type > feature_map_type;
    
    //maps a target pixel to a 2d point in the reduced feature space
    typedef vcl_map< vgl_point_2d<unsigned>, vgl_point_2d<double>, ncn1_vgl_point_2d_less_than > target_reduced_feature_map_2d_type;

    //maps a set of target/feature point pairs to a frame number
    typedef vcl_map< unsigned, target_reduced_feature_map_2d_type > frame_reduced_feature_map_2d_type;

}

using namespace feature_typedefs;

class ncn1_feature: public vbl_ref_count
{
public:

    //constructors
    ncn1_feature():is_ground_truth_(false),label_(0){}

    ncn1_feature( unsigned ndimensions ):nd_(ndimensions),is_ground_truth_(false),label_(0){}

    //processes
    void reduce_features_2d();

    //getters
    feature_map_type feature_map(){ return feature_map_; }

    frame_reduced_feature_map_2d_type feature_map_2d(){ return frame_feature_map_2d_; }

    unsigned ndimensions(){ return nd_; }

    unsigned label(){ return label_; }

    bool is_ground_truth(){ return is_ground_truth_; }

    //setters
    void set_ndimensions( unsigned const& nd ){ nd_ = nd; }

    void set_feature_map( feature_map_type& fm ) { feature_map_ = fm; }

    void set_label( unsigned& label ){ label_ = label; }

    void set_is_groud_truth( bool& gt ) { is_ground_truth_ = gt; }

    //save/load methods
    void save_feature_dat( vcl_string const& filename );

    void save_2d_pts_dat( vcl_string const& filename );
    
private:
    feature_map_type feature_map_;
    frame_reduced_feature_map_2d_type frame_feature_map_2d_;
    vnl_vector<double> normalized_singular_values_;
    unsigned nd_;
    bool is_ground_truth_;
    unsigned label_;
};

#endif //ncn1_feature_h_