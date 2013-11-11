//this is contrib/bm/dbcl/dbcl_classifier_factory.cxx

#include"dbcl_classifier_factory.h"

dbcl_classifier_factory::dbcl_classifier_factory( registered_classifiers const& classifier_type, dbcl_classifier::feature_map_type training_data, dbcl_classifier::feature_map_type test_data)
{
    switch(classifier_type)
    {
        case LINEAR_CLASSIFIER:
            classifier_ = new dbcl_linear_classifier(training_data,test_data);
            break;
        case TEMPORAL_STATE_MACHINE:
            classifier_ = new dbcl_state_machine_classifier(training_data,test_data);
                break;
        default:
            vcl_cerr << "ERROR: dbcl_classifier_factory -- unregistered classifier requested." << vcl_flush; 
            break;
    }
}//end dbcl_classifier_factory::dbcl_classifier_factory