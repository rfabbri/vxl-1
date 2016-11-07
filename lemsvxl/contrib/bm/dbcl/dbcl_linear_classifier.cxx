//this is contrib/bm/dcbl/dcbl_linear_classifier.cxx

#include"dbcl_linear_classifier.h"

dbcl_linear_classifier::dbcl_linear_classifier( feature_map_type& training_data, feature_map_type& test_data )
{
    training_data_ = training_data;
    test_data_ = test_data;

    feature_map_type::const_iterator training_data_itr;
    feature_map_type::const_iterator training_data_end = this->training_data_.end();

    vcl_set<unsigned> training_labels;

    //find the number of unique training labels
    for( training_data_itr = training_data_.begin(); training_data_itr != training_data_end; ++training_data_itr )
        training_labels.insert(training_data_itr->second->label());

    this->nclasses_ = training_labels.size();

}//end constructor


void dbcl_linear_classifier::build_X()
{
    feature_map_type::iterator fit = training_data_.begin();
    feature_map_type::iterator fend = training_data_.end();

    unsigned xdim = fit->second->feature_dim();

    X_.set_size(this->num_training_features(),xdim+1);

    unsigned row;
    for( fit = training_data_.begin(), row = 0; fit != fend; ++fit, ++row )
    {
        dbcl_temporal_feature::feature_vector_type fv = fit->second->feature_vector();

        X_[row][0] = 1;

        for( unsigned col = 0; col < xdim; ++col )
            X_[row][col+1] = fv[col];
    }//end training_data_ iteration
        
}//end dbcl_linear_classifier::build_X()

void dbcl_linear_classifier::build_T()
{
    T_.set_size(this->num_training_features(),this->nclasses_);
    T_.fill(double(0.0));

    feature_map_type::iterator fit;
    feature_map_type::iterator fend = this->training_data_.end();

    unsigned row;
    for( fit = this->training_data_.begin(), row = 0; fit != fend; ++fit, ++row ) 
        T_[row][fit->second->label()] = 1.0;

}//end dbcl_linear_classifier::build_T()

void dbcl_linear_classifier::compute_W()
{
    W_.set_size(this->X_.cols(),this->nclasses_);
    vnl_matrix<double> x_pseudo_inv = vnl_inverse(this->X_.transpose() * this->X_) * this->X_.transpose();
    W_ = x_pseudo_inv * this->T_;
}//end dbcl_linear_classifier::compute_W


void dbcl_linear_classifier::classify()
{
    //do this every time so that if new training points are added we may account for it.
    //Not very efficient probably could implement something like an observer pattern that updates
    //the matricies when a the training data changes.
    this->build_X();
    this->build_T();
    this->compute_W();

    feature_map_type::iterator fit;
    feature_map_type::iterator fend = test_data_.end();

    for( fit = test_data_.begin(); fit != fend; ++fit )
    {
        dbcl_temporal_feature_sptr current_test_feature = fit->second;
        vnl_vector<double> augmented_feature_vector(current_test_feature->feature_dim()+1,1);
        augmented_feature_vector.update(current_test_feature->feature_vector(),1);

        //vcl_cout << "augmented_feature_vector = " << augmented_feature_vector << '\n';

        vnl_vector<double> y = this->W_.transpose()*augmented_feature_vector;
            
        unsigned c = y.arg_max();

        current_test_feature->set_label( c );
        
    }//iterate through all test points


}//end dbcl_linear_classifier::classify