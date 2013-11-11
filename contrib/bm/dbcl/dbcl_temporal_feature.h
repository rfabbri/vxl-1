//this is contrib/bm/dbcl/dbcl_temporal_feature.h
#ifndef dbcl_temporal_feature_h_
#define dbcl_temporal_feature_h_
//:
// \file
// \date August 8, 2010
// \author Brandon A. Mayer
//
// \Class to store the training data for an individual target pixel
// \will associate a target point with a frame/feature map and training label.
//
// \verbatim
//  Modifications
// \endverbatim

#include<vbl/vbl_ref_count.h>
#include<vcl_map.h>
#include<vcl_vector.h>

#include<vgl/vgl_point_2d.h>

#include<vnl/vnl_vector.h>

class dbcl_temporal_feature : public vbl_ref_count
{
public:
    
    typedef vnl_vector<double> feature_vector_type;

    dbcl_temporal_feature(){}
    
    dbcl_temporal_feature( unsigned const& time, feature_vector_type const& feature_vector, unsigned label = 0): 
            time_(time), feature_vector_(feature_vector),label_(label){}

    ~dbcl_temporal_feature(){}

    //setters

    void set_time(unsigned& t){ time_ = t; }

    void set_feature_vector( feature_vector_type& feature_vector ) {feature_vector_ = feature_vector;}

    void set_feature_element( unsigned& dim, double& ele ){feature_vector_[dim] = ele;}

    void set_label( unsigned& label ){ label_ = label; }

    //getters

    unsigned time(){return time_;}

    feature_vector_type feature_vector() {return feature_vector_;}

    double feature_vector( unsigned dim ){return feature_vector_[dim];}

    unsigned label(){return label_;}

    unsigned feature_dim(){ return this->feature_vector_.size(); }

protected:
    unsigned time_; //could be frame id or unsigned clock etc.
    feature_vector_type feature_vector_;
    unsigned label_;
};

#endif //dbcl_temporal_training_point_h_