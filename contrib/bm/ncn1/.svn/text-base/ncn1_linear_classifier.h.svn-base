//this is contrib/bm/ncn1/ncn1_linear_classifier.h
#ifndef ncn1_linear_classifier_h_
#define ncn1_linear_classifier_h_

#include"ncn1_classifier.h"

#include<vcl_string.h>
#include<vcl_vector.h>

#include<vgl/vgl_point_2d.h>

#include<vnl/vnl_matrix.h>

class ncn1_linear_classifier: public ncn1_classifier
{
public:

    ncn1_linear_classifier(){type_="linear_classifier";}
     
    //ncn1_linear_classifier(vnl_matrix<bool> const& T, vnl_matrix<double> const& X):T_(T),X_(X){type_="linear_classifier";}

    ~ncn1_linear_classifier(){}

    //load training and test data from xml file
    bool parse_data_file( vcl_string const& filename ) { return true;}   

    //calculate parameters of the model using the training data then classify the test data
    void classify(){}

    vnl_matrix<double> X(){ return X_; }

    vnl_matrix<double> W(){ return W_; }

    vnl_matrix<bool> T(){ return T_; }

    vcl_vector<double> y();

    bool classify_point( vcl_vector<double> x );


private:
    vnl_matrix<bool> T_;
    vnl_matrix<double> X_;
    vnl_matrix<double> W_;
};

#endif //ncn1_linear_classifier_h_
