//this is contrib/bm/dbcl/dbcl_linear_classifier.h
#ifndef dbcl_linear_classifier_h_
#define dbcl_linear_classifier_h_
//:
// \file
// \date August 9, 2010
// \author Brandon A. Mayer
//
// Class to preform and store the results of a linear classifier applied to test data whos parameters are determined
// from the training data. X_ matrix is formed by training features, T_ the training labels and the weights are stored in W_.
// The results of the classifications will be stored on the label members of dcbl_test test_data which is a member of the base dbcl_classifier.
//
// uses 1-of-k coding sceme for T matrix
// y(x) = W^t x
//
// \verbatim
//  Modifications
// \endverbatim

#include"dbcl_classifier.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_set.h>

#include<vnl/vnl_matrix.h>
#include<vnl/vnl_inverse.h>

class dbcl_linear_classifier : public dbcl_classifier
{
public:
    //dbcl_linear_classifier();

    dbcl_linear_classifier( feature_map_type& training_data, feature_map_type& test_data);
      //{training_data_ = training_data; test_data_ = test_data; nclasses_ = nclasses; type_ = "LINEAR_CLASSIFIER";}

    ~dbcl_linear_classifier(){}

    //the classification of each point will be stored on the temporal feature class
    virtual void classify();

    //getters
    vnl_matrix<double> X(){ return X_; }

    vnl_matrix<double> T(){ return T_; }

    vnl_matrix<double> W(){ return W_; }

    //processing
    void compute_W();

    void build_X();

    void build_T();

protected:
    unsigned nclasses_;
    vnl_matrix<double> X_;
    vnl_matrix<double> T_;
    vnl_matrix<double> W_;
};

#endif //dbcl_linear_classifier_h_