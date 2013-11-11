// This is /lemsvxl/brcv/rec/dborl/algo/vox_convert_edg_to_img_and_orient/vox_convert_edg_to_img_and_orient_params.h

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

#ifndef VOX_CONVERT_EDG_TO_IMG_AND_ORIENT_PARAMS_H_
#define VOX_CONVERT_EDG_TO_IMG_AND_ORIENT_PARAMS_H_

#include <dborl/algo/dborl_algo_params.h>

class vox_convert_edg_to_img_and_orient_params : public dborl_algo_params
{
public:

    //: Constructor
    vox_convert_edg_to_img_and_orient_params(vcl_string algo_name);

    // MEMBER VARIABLES

    //: Name of input object
    dborl_parameter<vcl_string> input_object_name_;

    dborl_parameter<vcl_string> input_edgemap_dir_;
    dborl_parameter<vcl_string> input_linked_edgemap_dir_;
    dborl_parameter<vcl_string> input_edgemap_extension_;
    dborl_parameter<vcl_string> input_linked_edgemap_extension_;

    dborl_parameter<bool> use_linked_;
    dborl_parameter<bool> use_pyramid_;
    dborl_parameter<bool> is_gray_;

    // if written to this folder as opposed to object folder then the shock graph
    // gets associated to the input object.
    // if nothing is written here, nothing gets associated
    dborl_parameter<vcl_string> output_edge_image_orient_dir_;
    dborl_parameter<vcl_string> output_edge_image_extension_;
    dborl_parameter<vcl_string> output_edge_orient_extension_;

};

#endif /* VOX_CONVERT_EDG_TO_IMG_AND_ORIENT_PARAMS_H_ */
