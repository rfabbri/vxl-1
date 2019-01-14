//this is contrib/bm/ncn1/ncn1_classifier.h
#ifndef ncn1_classifier_h_
#define ncn1_classifier_h_

#include<vbl/vbl_ref_count.h>

#include<string>

class ncn1_classifier: public vbl_ref_count
{
public:
    //ncn1_classifier(){}

    //ncn1_classifier( const std::string& type ):type_(type){}

    //function to load training and test data from xml file
    virtual bool parse_data_file( std::string const& filename ) = 0;

    virtual void classify() = 0;

    std::string type(){ return type_; }

    ~ncn1_classifier(){}

protected:
    std::string type_;
};

#endif //ncn1_classifier_h_