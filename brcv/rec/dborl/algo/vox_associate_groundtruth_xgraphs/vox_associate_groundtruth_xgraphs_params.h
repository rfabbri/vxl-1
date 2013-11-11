// This is /lemsvxl/brcv/rec/dborl/algo/vox_train_xgraph_geometry_and_ccm_models/vox_associate_groundtruth_xgraphs/vox_associate_groundtruth_xgraphs_params.h

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 5, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#ifndef VOX_ASSOCIATE_GROUNDTRUTH_XGRAPHS_PARAMS_H_
#define VOX_ASSOCIATE_GROUNDTRUTH_XGRAPHS_PARAMS_H_

#include <dborl/algo/dborl_algo_params.h>

class vox_associate_groundtruth_xgraphs_params : public dborl_algo_params
{
public:

    //: Constructor
    vox_associate_groundtruth_xgraphs_params(vcl_string algo_name);

    // MEMBER VARIABLES

    //: Name of input object
    dborl_parameter<vcl_string> input_object_name_;

    // if written to this folder as opposed to object folder then the shock graph
    // gets associated to the input object.
    // if nothing is written here, nothing gets associated
    dborl_parameter<vcl_string> output_xgraph_dir_;
    dborl_parameter<vcl_string> input_xgraph_list_file_;

};

#endif /* VOX_ASSOCIATE_GROUNDTRUTH_XGRAPHS_PARAMS_H_ */
