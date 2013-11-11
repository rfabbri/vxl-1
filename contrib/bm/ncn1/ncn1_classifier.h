//this is contrib/bm/ncn1/ncn1_classifier.h
#ifndef ncn1_classifier_h_
#define ncn1_classifier_h_

#include<vbl/vbl_ref_count.h>

#include<vcl_string.h>

class ncn1_classifier: public vbl_ref_count
{
public:
    //ncn1_classifier(){}

    //ncn1_classifier( const vcl_string& type ):type_(type){}

    //function to load training and test data from xml file
    virtual bool parse_data_file( vcl_string const& filename ) = 0;

    virtual void classify() = 0;

    vcl_string type(){ return type_; }

    ~ncn1_classifier(){}

protected:
    vcl_string type_;
};

#endif //ncn1_classifier_h_