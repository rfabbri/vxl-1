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

#ifndef VOX_TRAIN_XGRAPH_geom_MODEL_PARAMS_H_
#define VOX_TRAIN_XGRAPH_geom_MODEL_PARAMS_H_

#include <dborl/algo/dborl_algo_params.h>
#include <dbsks/pro/dbsks_train_xshock_geom_model_process.h>

class vox_train_xgraph_geom_model_params : public dborl_algo_params
{
public:

    //: Constructor
    vox_train_xgraph_geom_model_params(vcl_string algo_name);

    // MEMBER VARIABLES

    //parameter for the index file
    //Is a flat image database
    dborl_parameter<vcl_string> index_filename_;

    dborl_parameter<vcl_string> input_gt_objects_file_;

    dborl_parameter<vcl_string> xgraph_assoc_label_;

    dborl_parameter<vcl_string> temp_dir_;

    dborl_parameter<vcl_string> output_geom_dir_;
};

#endif /* VOX_TRAIN_XGRAPH_geom_MODEL_PARAMS_H_ */
