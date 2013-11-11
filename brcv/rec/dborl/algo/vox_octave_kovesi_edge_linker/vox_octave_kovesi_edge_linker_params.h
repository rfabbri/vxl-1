// This is /lemsvxl/brcv/rec/dborl/algo/vox_octave_kovesi_edge_linker/vox_octave_kovesi_edge_linker_params.h

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 2, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#ifndef VOX_OCTAVE_KOVESI_EDGE_LINKER_PARAMS_H_
#define VOX_OCTAVE_KOVESI_EDGE_LINKER_PARAMS_H_

#include <dborl/algo/dborl_algo_params.h>
#include <dbdet/pro/dbdet_kovesi_edge_linker_process.h>

class vox_octave_kovesi_edge_linker_params : public dborl_algo_params
{
public:

    //: Constructor
    vox_octave_kovesi_edge_linker_params(vcl_string algo_name);

    // MEMBER VARIABLES

    //: Name of input object
    dborl_parameter<vcl_string> input_object_name_;

    //: passes the folder of the input object
    dborl_parameter<vcl_string> input_object_dir_;

    //: extension of the input edge map
    dborl_parameter<vcl_string> input_edgemap_extension_;

    //: extension of the output edge map
    dborl_parameter<vcl_string> output_edgemap_extension_;

    //: extension of the cem file
    dborl_parameter<vcl_string> output_cemv_extension_;

    // if written to this folder as opposed to object folder then the shock graph
    // gets associated to the input object.
    // if nothing is written here, nothing gets associated
    dborl_parameter<vcl_string> output_linked_edge_dir_;

    dborl_parameter<vcl_string> input_edgemap_dir_;

    dborl_parameter<bool> use_pyramid_;
};

#endif /* VOX_OCTAVE_KOVESI_EDGE_LINKER_PARAMS_H_ */
