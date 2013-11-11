//this is contrib/bm/ncn1/ncn1_classifier_factory.h
#ifndef ncn1_classifier_factory_h_
#define ncn1_classifier_factory_h_

#include"ncn1_classifier_sptr.h"

#include"ncn1_linear_classifier.h"

class ncn1_classifier_factory
{
public:
    enum registered_classifiers {LINEAR_CLASSIFIER};

    ncn1_classifier_factory(registered_classifiers const& classifier_type, vcl_string const& xml_file);

    ~ncn1_classifier_factory(){}

private:
    ncn1_classifier_sptr classifier_;
};

#endif //ncn1_classifier_factory_h_