// This is /lemsvxl/brcv/rec/dborl/algo/vox_octave_pb_edge_detector/vox_octave_pb_edge_detector_params.h

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Feb 26, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#ifndef VOX_OCTAVE_PB_EDGE_DETECTOR_PARAMS_H_
#define VOX_OCTAVE_PB_EDGE_DETECTOR_PARAMS_H_

#include <dborl/algo/dborl_algo_params.h>
#include <dbdet/pro/dbdet_pb_edge_detector_process.h>

class vox_octave_pb_edge_detector_params : public dborl_algo_params
{
public:

    //: Constructor
    vox_octave_pb_edge_detector_params(vcl_string algo_name);

    // MEMBER VARIABLES

    //: Name of input object
    dborl_parameter<vcl_string> input_object_name_;

    //: passes the folder of the input object
    dborl_parameter<vcl_string> input_object_dir_;

    //: extension of the input image
    dborl_parameter<vcl_string> input_extension_;

    //: extension of the output edge map
    dborl_parameter<vcl_string> edgemap_extension_;

    // if written to this folder as opposed to object folder then the shock graph
    // gets associated to the input object.
    // if nothing is written here, nothing gets associated
    dborl_parameter<vcl_string> output_edgemap_dir_;

    dborl_parameter<vcl_string> input_pyramid_dir_;

    dborl_parameter<bool> use_pyramid_;

    dborl_parameter<int> min_height_;

    dborl_parameter<int> min_width_;

    dborl_parameter<int> max_height_;

    dborl_parameter<int> max_width_;
};

#endif /* VOX_OCTAVE_PB_EDGE_DETECTOR_PARAMS_H_ */
