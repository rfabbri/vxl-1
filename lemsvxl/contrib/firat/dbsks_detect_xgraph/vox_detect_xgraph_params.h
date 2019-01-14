// This is dborl/algo/vox_detect_xgraph/vox_detect_xgraph_params.h

#ifndef vox_detect_xgraph_params_h_
#define vox_detect_xgraph_params_h_

//:
// \file
// \brief parameter set for detect_xgraph algorithm
//
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 2, 2008
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#include <dborl/algo/dborl_algo_params.h>


//==============================================================================
// vox_detect_xgraph_params
//==============================================================================

//: Parameters of detect_xgraph algorithm
class vox_detect_xgraph_params : public dborl_algo_params
{
    public:
        //: Constructor
        vox_detect_xgraph_params(std::string algo_name);

        // Parameter parsers ---------------------------------------------------------

        //: List of boundary contour fragment to ignore
        bool parse_input_cfrag_list_to_ignore(std::vector<std::string >& cfrag_list_to_ignore) const;

        std::string get_image_file() const;
        std::string get_xgraph_file() const;
        std::string get_edgemap_file() const;
        //std::string get_edgeorient_file() const;
        std::string get_xgraph_geom_file() const;
        std::vector<double > get_xgraph_scales() const; // depreciated
        std::string get_xgraph_course_ccm_file() const;
        std::string get_xgraph_fine_ccm_file() const;
        //std::string get_cemv_file() const;
        std::vector<std::string > get_cfrag_list_to_ignore() const;
        std::string get_output_folder() const; // location of all output files

        std::string get_xgraph_prototype_name() const; //> (only) name of the xgraph prototype file

        // MEMBER VARIABLES ----------------------------------------------------------

        //>> Object <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        //: Name of input object
        dborl_parameter<std::string> input_object_name_;

        //: passes the folder of the input object
        dborl_parameter<std::string> input_object_dir_;

        //>> image <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        //: Get input image from object folder?
        dborl_parameter<bool> input_image_from_object_folder_;

        //: folder containing the edgemaps (if input is not from object directory)
        dborl_parameter<std::string> input_image_folder_;

        //: extension of the input image
        dborl_parameter<std::string> input_image_extension_;


        //>> edgemap
        //: Get input Pb edgemap from object folder?
        dborl_parameter<bool> input_edgemap_from_object_folder_;

        //: folder containing the edgemaps (if input is not from object directory)
        dborl_parameter<std::string> input_edgemap_folder_;

        //: extension of the input image
        dborl_parameter<std::string> input_edgemap_extension_;

        ////: Folder of input edgemap orientation (if not in object folder)
        //dborl_parameter<std::string > input_edgeorient_folder_;

        //: extension of the input edge orientation
        dborl_parameter<std::string > input_edgeorient_extension_;

        ////: Folder of input linked-edge folder (if not in object folder)
        //dborl_parameter<std::string > input_cemv_folder_;

        //: extension of the linked-edge file
        dborl_parameter<std::string > input_cemv_extension_;

        //: Relative scale of edgemap used for detection.
        // value is in log2 of the scale ratio between of the selected edgemap and the selected image in the pyramid
        dborl_parameter<float > input_edgemap_log2_scale_ratio_;



        //>> Trained model for xgraph
        //: Folder containing the object shock graph
        dborl_parameter<std::string > input_xgraph_folder_;

        //: xgraph file name
        dborl_parameter<std::string > input_xgraph_prototype_filename_;

        //>> xgraph geom model

        //: xgraph geometric model filename (.xml)
        dborl_parameter<std::string > input_xgraph_geom_filename_;

        //>> xgraph ccm model

        //: xgraph Contour-Chamfer-Matching model filename (.xml)
        dborl_parameter<std::string > input_xgraph_course_ccm_filename_;

        //: xgraph Contour-Chamfer-Matching model filename (.xml)
        dborl_parameter<std::string > input_xgraph_fine_ccm_filename_;

        //: List of contour fragments whose cost will be ignored
        dborl_parameter<std::string > input_cfrag_list_to_ignore_;

        //: Parameter lambda of Oriented Chamfer Matching
        dborl_parameter<float > input_wcm_weight_unmatched_;

        //>> Detection objectives <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        //: Object categery of input shock graph
        dborl_parameter<std::string > input_model_category_;



        //> List of xgraph sizes to run detection on

        //: Min graph size
        dborl_parameter<float > prototype_xgraph_min_size_;

        //: Max graph size
        dborl_parameter<float > prototype_xgraph_max_size_;

        //: Increment step of graph size - computed as log2 of scale ratio
        dborl_parameter<float > prototype_xgraph_log2_increment_step_;

        //: Max ratio between largest and smallest model size - in log2
        dborl_parameter<float > prototype_xgraph_ratio_max_size_to_min_size_;



        // Output

        //: Save result to the object folder?
        dborl_parameter<bool> save_to_object_folder_;

        // if written to this folder as opposed to object folder then the shock graph
        // gets associated to the input object.
        // if nothing is written here, nothing gets associated
        dborl_parameter<std::string> output_object_folder_;

        //: tag for this algorithm
        std::string algo_abbreviation_;

        // External constraints

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

#endif  //_vox_detect_xgraph_params_h
