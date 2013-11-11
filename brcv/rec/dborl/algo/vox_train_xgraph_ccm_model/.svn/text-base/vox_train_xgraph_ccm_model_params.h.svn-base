// This is /lemsvxl/brcv/rec/dborl/algo/vox_train_xgraph_geometry_and_ccm_models/vox_train_xgraph_ccm_model/vox_train_xgraph_ccm_model_params.h

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 9, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#ifndef VOX_TRAIN_XGRAPH_CCM_MODEL_PARAMS_H_
#define VOX_TRAIN_XGRAPH_CCM_MODEL_PARAMS_H_

#include <dborl/algo/dborl_algo_params.h>
#include <dbsks/pro/dbsks_train_xshock_geom_model_process.h>
#include "vox_train_xshock_ccm_model_process.h"

class vox_train_xgraph_ccm_model_params : public dborl_algo_params
{
public:

    //: Constructor
    vox_train_xgraph_ccm_model_params(vcl_string algo_name);

    // MEMBER VARIABLES

    //parameter for the index file
    //Is a flat image database
    dborl_parameter<vcl_string> index_filename_;

    dborl_parameter<vcl_string> output_ccm_dir_;

    dborl_parameter<vcl_string> matlab_file_extraction_path_;

    dborl_parameter<vcl_string> bfrags_to_ignore_;
};

#endif /* VOX_TRAIN_XGRAPH_CCM_MODEL_PARAMS_H_ */
