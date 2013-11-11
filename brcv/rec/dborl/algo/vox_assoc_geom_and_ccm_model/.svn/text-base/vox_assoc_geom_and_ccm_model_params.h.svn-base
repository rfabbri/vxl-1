// This is /lemsvxl/brcv/rec/dborl/algo/vox_train_xgraph_geometry_and_ccm_models/vox_assoc_geom_and_ccm_model/vox_assoc_geom_and_ccm_model_params.h

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Apr 5, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#ifndef vox_assoc_geom_and_ccm_model_PARAMS_H_
#define vox_assoc_geom_and_ccm_model_PARAMS_H_

#include <dborl/algo/dborl_algo_params.h>

class vox_assoc_geom_and_ccm_model_params : public dborl_algo_params
{
public:

    //: Constructor
    vox_assoc_geom_and_ccm_model_params(vcl_string algo_name);

    // MEMBER VARIABLES


    // if written to this folder as opposed to object folder then the shock graph
    // gets associated to the input object.
    // if nothing is written here, nothing gets associated
    dborl_parameter<vcl_string> output_ccm_dir_;
    dborl_parameter<vcl_string> output_geom_dir_;
    dborl_parameter<vcl_string> input_ccm_;
    dborl_parameter<vcl_string> input_geom_;

};

#endif /* vox_assoc_geom_and_ccm_model_PARAMS_H_ */
