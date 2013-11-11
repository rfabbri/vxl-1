//:
// \file
// \brief Parameters for the edge detection evaluator
//
// \author Amir Tamrakar
// \date 08/01/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_eval_edge_dets_params_h)
#define _dborl_eval_edge_dets_params_h

#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_eval_edge_dets_params : public dborl_algo_params
{
public:
  
  //io parameters
  dborl_parameter<vcl_string> input_object_name_;

  dborl_parameter<vcl_string> input_object_dir_;    // passes the folder of the input object
  dborl_parameter<vcl_string> input_extension_;     // extension of the input image
  dborl_parameter<vcl_string> GT_extension_;        // extension of the Ground Truth image

  dborl_parameter<vcl_string> edg_output_extension_;    //extension of the edge file
  dborl_parameter<vcl_string> ROC_output_extension_;    //extension of the ROC file

  dborl_parameter<vcl_string> output_dir_;          // if written to this folder as opposed to object folder then the edge map gets associated to the input object
                                                    // if nothing is written here, nothing gets associated

  vcl_string algo_abbreviation_;

  //general parameters
  dborl_parameter<bool> save_edges_; 
  dborl_parameter<bool> save_to_object_folder_; 
  
  //edge detection parameters
  dborl_parameter<int> edge_det_type_;       //detector option
  dborl_parameter<double> edge_det_sigma_;   //sigma
  dborl_parameter<double> edge_det_thresh_;  //threshold
  dborl_parameter<int> edge_det_N_;          //interpolation depth

  //: constructor
  dborl_eval_edge_dets_params(vcl_string algo_name) : dborl_algo_params(algo_name) 
  { 
    algo_abbreviation_ = "gel_eval";

    input_object_name_.set_values(param_list_, "io", "input_object_name", "Input object name", "dummy", "dummy", 
      0,   // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    input_object_dir_.set_values(param_list_, "io", "input_object_dir", "input object", "", 
      "/vision/images/misc/object/", 
      0,   // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_DIR); 

    input_extension_.set_values(param_list_, "io", "input_extension", "input image extension", ".pgm", ".pgm");
    GT_extension_.set_values(param_list_, "io", "GT_extension", "GT image extension", ".pgm.gtf.pgm", ".pgm.gtf.pgm");

    edg_output_extension_.set_values(param_list_, "io", "edg_output_extension", "edge output extension", ".edg", ".edg");
    ROC_output_extension_.set_values(param_list_, "io", "ROC_output_extension", "ROC output extension", ".roc_edg", ".roc_edg");
    
    output_dir_.set_values(param_list_,  "io",  "output_edge_folder",  "output folder to write edge map", "", 
      "/vision/projects/kimia/categorization/output/", 
      0,   // associate to 0th input object
      dborl_parameter_system_info::OUTPUT_FILE, "edge_map", dborl_parameter_type_info::FILEASSOC);

    // set default parameters
    save_edges_.set_values(param_list_, "io", "save_edges", "Save edges?", true, false);
    save_to_object_folder_.set_values(param_list_, "io", "save_to_object_folder", "save to object folder?", false, false);

    // set edge detection parameters
    edge_det_type_.set_values(param_list_, "edge_det", "det_type", "edge detector: [1]=gen, [2]=TO", 2, 2); 
    edge_det_sigma_.set_values(param_list_, "edge_det", "det_sigma", "edge detector sigma", 1.0, 1.0);
    edge_det_thresh_.set_values(param_list_, "edge_det", "det_thresh", "edge response threshold", 2.0, 1.0);
    edge_det_N_.set_values(param_list_, "edge_det", "det_interp_N", "Interpolation depth", 1, 1);

  }

};

#endif  //_dborl_eval_edge_dets_params_h
