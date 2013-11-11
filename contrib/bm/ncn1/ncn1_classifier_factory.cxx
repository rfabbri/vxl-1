//this is contrib/bm/ncn1_classifier_factory.cxx
#include"ncn1_classifier_factory.h"

ncn1_classifier_factory::ncn1_classifier_factory(registered_classifiers const& classifier_type, vcl_string const& xml_file)
{
    switch(classifier_type)
    {
    case LINEAR_CLASSIFIER: 
        classifier_ = new ncn1_linear_classifier;
        break;
    default:
        break;
    }
}//end default constructor