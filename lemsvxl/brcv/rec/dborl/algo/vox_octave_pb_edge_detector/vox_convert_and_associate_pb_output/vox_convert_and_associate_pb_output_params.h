//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 29, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#ifndef vox_convert_and_associate_pb_output_PARAMS_H_
#define vox_convert_and_associate_pb_output_PARAMS_H_

#include <dborl/algo/dborl_algo_params.h>

class vox_convert_and_associate_pb_output_params : public dborl_algo_params
{
public:

    //: Constructor
    vox_convert_and_associate_pb_output_params(vcl_string algo_name);

    // MEMBER VARIABLES

    //: Name of input object
    dborl_parameter<vcl_string> input_object_name_;

    // if written to this folder as opposed to object folder then the shock graph
    // gets associated to the input object.
    // if nothing is written here, nothing gets associated
    dborl_parameter<vcl_string> output_edgemap_dir_;
    dborl_parameter<vcl_string> input_edgemap_folder_list_file_;

    //: extension of the output edge map
    dborl_parameter<vcl_string> input_edgemap_extension_;
    dborl_parameter<vcl_string> input_edgeorient_extension_;
    dborl_parameter<vcl_string> output_edgemap_extension_;

    dborl_parameter<int> min_height_;
    dborl_parameter<int> min_width_;

};

#endif /* vox_convert_and_associate_pb_output_PARAMS_H_ */
