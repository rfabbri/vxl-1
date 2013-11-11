//this is contrib/bm/dbcl/dbcl_classifier.h
#ifndef dbcl_classifier_h_
#define dbcl_classifier_h_
//:
// \file
// \date August 9, 2010
// \author Brandon A. Mayer
//
// Classifier base class. dbcl_xxx_classifiers will inherit and implement the virtual classify function
// This class also stores the training/test data
//
// \verbatim
//  Modifications
// \endverbatim

#include"dbcl_temporal_feature_sptr.h"

//#include"dbcl_data.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_iostream.h>
#include<vcl_map.h>
#include<vcl_string.h>
#include<vcl_utility.h>

class dbcl_classifier : public vbl_ref_count
{
public:

    //maps frame number and a feature smart pointer
    typedef vcl_map<unsigned, dbcl_temporal_feature_sptr> feature_map_type;

    virtual void classify() = 0;

    vcl_string type(){ return type_; }

    ~dbcl_classifier(){}

    //setters
    
    void set_training_data( feature_map_type& fm ){ training_data_ = fm; }

    void add_training_feature( dbcl_temporal_feature_sptr tf );

    bool set_training_label( unsigned const& time, unsigned& label );

    void set_test_data( feature_map_type& fm ){ test_data_ = fm;}

    void add_test_feature( dbcl_temporal_feature_sptr tf );

    bool set_test_label( unsigned const& time, unsigned& label );

    //getters
    bool training_label( unsigned const& time, unsigned& label );

    feature_map_type training_data(){ return training_data_; }

    bool training_feature_vector( unsigned const& time, dbcl_temporal_feature::feature_vector_type& feature_vector );

    bool training_feature_label( unsigned const& time, dbcl_temporal_feature::feature_vector_type& feature_vector, unsigned& label );

    unsigned num_training_features(){ return this->training_data_.size(); }

    bool test_label( unsigned const& time, unsigned& label );

    feature_map_type test_data(){ return test_data_; }

    bool test_feature_vector( unsigned const& time, dbcl_temporal_feature::feature_vector_type& feature_vector );

    bool test_feature_label( unsigned const& time, dbcl_temporal_feature::feature_vector_type& feature_vector, unsigned& label );

    unsigned num_test_features(){ return this->test_data_.size(); }

    unsigned feature_dim() { return this->test_data_.begin()->second->feature_dim(); }

protected:
    vcl_string type_;

    //setters
    void add_feature( dbcl_temporal_feature_sptr tf, feature_map_type& fm );
    bool set_label( unsigned const& time, unsigned& label, feature_map_type& fm );

    //getters
    bool label( unsigned const& time, unsigned& label, feature_map_type& fm );
    bool feature_vector( unsigned const& time, dbcl_temporal_feature::feature_vector_type& feature_vector, feature_map_type& fm );
    bool feature_label( unsigned const& time, dbcl_temporal_feature::feature_vector_type& feature_vector, unsigned& label, feature_map_type& fm);
    
    feature_map_type training_data_;
    feature_map_type test_data_;
};

#endif //dbcl_classifier_h_