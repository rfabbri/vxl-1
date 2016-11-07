// This is dborl/algo/vox_detect_xgraph2/vox_detect_xgraph2_params.h

#ifndef vox_detect_xgraph2_params_h_
#define vox_detect_xgraph2_params_h_

//:
// \file
// \brief parameter set for detect_xgraph algorithm
//
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Mar 19, 2010
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#include <dborl/algo/dborl_algo_params.h>


//==============================================================================
// vox_detect_xgraph2_params
//==============================================================================

//: Parameters of detect_xgraph algorithm
class vox_detect_xgraph2_params : public dborl_algo_params
{
public:
    //: Constructor
    vox_detect_xgraph2_params(vcl_string algo_name);

    // Parameter parsers ---------------------------------------------------------

    //: List of boundary contour fragment to ignore
    bool parse_input_cfrag_list_to_ignore(vcl_vector<vcl_string >& cfrag_list_to_ignore) const;

    vcl_string get_image_file() const;
    vcl_string get_xgraph_file() const;
    //vcl_string get_edgeorient_file() const;
    vcl_string get_xgraph_geom_file() const;
    vcl_vector<double > get_xgraph_scales() const; // depreciated
    vcl_string get_xgraph_ccm_file() const;
    //vcl_string get_cemv_file() const;
    vcl_vector<vcl_string > get_cfrag_list_to_ignore() const;
    vcl_string get_output_folder() const; // location of all output files

    vcl_string get_xgraph_prototype_name() const; //> (only) name of the xgraph prototype file

    // MEMBER VARIABLES ----------------------------------------------------------

    //parameter for the index file
    //Is a flat image database
    dborl_parameter<vcl_string> index_filename_;

    //>> Object <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    //: Name of input object
    dborl_parameter<vcl_string> input_object_name_;

    //: passes the folder of the input object
    dborl_parameter<vcl_string> input_object_dir_;

    //: extension of the input image
    dborl_parameter<vcl_string> input_image_extension_;

    //: folder containing the edgemaps (if input is not from object directory)
    dborl_parameter<vcl_string> input_edgemap_folder_;

    //: extension of the input image
    dborl_parameter<vcl_string> input_edgemap_extension_;

    ////: Folder of input edgemap orientation (if not in object folder)
    //dborl_parameter<vcl_string > input_edgeorient_folder_;

    //: extension of the input edge orientation
    dborl_parameter<vcl_string > input_edgeorient_extension_;

    ////: Folder of input linked-edge folder (if not in object folder)
    //dborl_parameter<vcl_string > input_cemv_folder_;

    //: Relative scale of edgemap used for detection.
    // value is in log2 of the scale ratio between of the selected edgemap and the selected image in the pyramid
    dborl_parameter<float > input_edgemap_log2_scale_ratio_;



    //>> Trained model for xgraph
    //: Folder containing the object shock graph
    dborl_parameter<vcl_string > input_gt_xgraph_label_;

    //: xgraph file name
    dborl_parameter<vcl_string > input_xgraph_prototype_object_;

    //>> xgraph geom model

    //: xgraph geometric model filename (.xml)
    dborl_parameter<vcl_string > input_xgraph_geom_dir_;

    //>> xgraph ccm model

    //: xgraph Contour-Chamfer-Matching model filename (.xml)
    dborl_parameter<vcl_string > input_xgraph_ccm_dir_;

    //: List of contour fragments whose cost will be ignored
    dborl_parameter<vcl_string > input_cfrag_list_to_ignore_;

    //: Parameter lambda of Oriented Chamfer Matching
    dborl_parameter<float > input_wcm_weight_unmatched_;

    //>> Detection objectives <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    //: Object categery of input shock graph
    dborl_parameter<vcl_string > input_model_category_;



    //> List of xgraph sizes to run detection on

    ////: [Depreciated] Scales of objects in the image (square root of area)
    //dborl_parameter<vcl_string > input_model_scales_;

    //: Base size of prototype xgraph - all detections will be run on this size
    // \todo This will be detetermined by the ccm xgraph model
    dborl_parameter<float > prototype_xgraph_base_size_;

    //: Min graph size
    dborl_parameter<float > prototype_xgraph_min_size_;

    //: Max graph size
    dborl_parameter<float > prototype_xgraph_max_size_;

    //: Increment step of graph size - computed as log2 of scale ratio
    dborl_parameter<float > prototype_xgraph_log2_increment_step_;

    //: Max ratio between largest and smallest model size - in log2
    dborl_parameter<float > prototype_xgraph_ratio_max_size_to_min_size_;



    // Output

    // if written to this folder as opposed to object folder then the shock graph
    // gets associated to the input object.
    // if nothing is written here, nothing gets associated
    dborl_parameter<vcl_string> output_object_folder_;

    //: tag for this algorithm
    vcl_string algo_abbreviation_;

    //>> External constraints
    //: Width of detection window
    dborl_parameter<int > detection_window_width_;

    //: Height of detection window
    dborl_parameter<int > detection_window_height_;

    //: (Absolute) Minimum confidence value to be accepted as a detection
    dborl_parameter<float > min_accepted_confidence_;

    //: Run non-max suppression based on overlapping?
    dborl_parameter<bool > run_nms_based_on_overlap_;

    //: Minimum overlap between two detection before one is rejected
    dborl_parameter<float > min_overlap_ratio_for_rejection_;

};

#endif  //_vox_detect_xgraph2_params_h
