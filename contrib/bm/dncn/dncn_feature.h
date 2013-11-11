//this is contrib/bm/dncn/dncn_feature.h
#ifndef dncn_feature_h_
#define dncn_feature_h_

#include<vbl/vbl_ref_count.h>

#include<vnl/vnl_vector.h>

class dncn_feature: public vbl_ref_count
{
public:
    typedef vnl_vector<double> feature_vector_type;

    dncn_feature():is_gt_(false),label_(0),frame_(0){}

    dncn_feature( feature_vector_type& feature_vector, unsigned& frame, bool is_gt = false, unsigned label = 0): 
        feature_vector_(feature_vector), frame_(frame), is_gt_(is_gt), label_(label){}

    ~dncn_feature(){}

    //setters
    void set_feature_vector( feature_vector_type& fv ){ feature_vector_ = fv; }

    void set_frame( unsigned const& frame ){ frame_ = frame; }

    void set_is_gt( bool const& is_gt ){ is_gt_ = is_gt; }

    void set_label( unsigned& label ){ label_ = label; }

    void set_feature_vector_reduced( feature_vector_type& fv_reduced ){ feature_vector_reduced_ = fv_reduced; }

    //getters
    feature_vector_type feature_vector(){ return feature_vector_; }

    unsigned frame(){ return frame_; }

    bool is_gt(){ return is_gt_; }

    unsigned label(){ return label_; }

    feature_vector_type feature_vector_reduced(){ return feature_vector_reduced_; }

    feature_vector_type::iterator feature_vector_begin(){ return this->feature_vector_.begin(); }

    feature_vector_type::iterator feature_vector_end(){ return  this->feature_vector_.end(); }

    feature_vector_type::iterator feature_vector_reduced_begin(){ return this->feature_vector_reduced_.begin(); }

    feature_vector_type::iterator feature_vector_reduced_end(){ return this->feature_vector_reduced_.end(); }

protected:
    feature_vector_type feature_vector_;
    feature_vector_type feature_vector_reduced_;
    bool is_gt_;
    unsigned label_;
    unsigned frame_;
};
#endif //dncn_feature_h_
