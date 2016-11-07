//this is contrib/bm/dbcl/dbcl_classifier_factory.h
#ifndef dbcl_classifier_factory_h_
#define dbcl_classifier_factory_h_

#include"dbcl_classifier_sptr.h"

#include"dbcl_linear_classifier_sptr.h"
#include"dbcl_state_machine_classifier.h"

enum registered_classifiers {LINEAR_CLASSIFIER, TEMPORAL_STATE_MACHINE};

class dbcl_classifier_factory
{
public:
    

    dbcl_classifier_factory( vcl_string const& xml_file );

    dbcl_classifier_factory( registered_classifiers const& classifier_type, dbcl_classifier::feature_map_type training_data, dbcl_classifier::feature_map_type test_data);

   // dbcl_classifier_factory( registered_classifiers const& classifier_type, dncn_

    ~dbcl_classifier_factory(){}

    dbcl_classifier_sptr classifier(){ return classifier_; }

private:
    dbcl_classifier_sptr classifier_;
};

#endif //dbcl_classifier_factory_h_