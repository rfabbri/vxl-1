//:
// \file
// \brief Parameters for the sel edge linker evaluator
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

#if !defined(_dborl_eval_sel_edge_linker_params_h)
#define _dborl_eval_sel_edge_linker_params_h

#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_eval_sel_edge_linker_params : public dborl_algo_params
{
public:
  
  //io parameters
  dborl_parameter<vcl_string> input_object_name_;

  dborl_parameter<vcl_string> input_object_dir_;    // passes the folder of the input object
  dborl_parameter<vcl_string> input_extension_;     // extension of the input image
  dborl_parameter<vcl_string> GT_extension_;        // extension of the Ground Truth image

  dborl_parameter<vcl_string> edg_output_extension_;    //extension of the edge file
  dborl_parameter<vcl_string> cem_output_extension_;    //extension of the contour file
  dborl_parameter<vcl_string> ROC_output_extension_;    //extension of the ROC file

  dborl_parameter<vcl_string> output_dir_;          // if written to this folder as opposed to object folder then the edge map gets associated to the input object
                                                    // if nothing is written here, nothing gets associated

  vcl_string algo_abbreviation_;

  //general parameters

  dborl_parameter<bool> save_edges_;  
  dborl_parameter<bool> save_contours_;
  dborl_parameter<bool> save_to_object_folder_; 
  
  //edge detection parameters
  dborl_parameter<int> edge_det_type_;       //detector option
  dborl_parameter<double> edge_det_sigma_;   //sigma
  dborl_parameter<double> edge_det_thresh_;  //threshold
  dborl_parameter<int> edge_det_N_;          //interpolation depth

  //edge linker parameters
  dborl_parameter<int> curve_model_type_;    //curve model type
  dborl_parameter<double> nrad_;             //neighborhood radius
  dborl_parameter<double> dt_;               //default dt
  dborl_parameter<double> dx_;               //default dx
  dborl_parameter<bool> badap_uncer_;        //use uncertainty from edges
  dborl_parameter<double> token_len_;        //token length
  dborl_parameter<double> max_k_;            //max curvature
  dborl_parameter<double> max_gamma_;        //max curvature derivative
  dborl_parameter<int> cvlet_type_;          //curvelet grouping type

  dborl_parameter<int> hybrid_method_;       //if using hybrid methods, select the subset of hybrid methods 
  dborl_parameter<double> len_thresh_;       //base length threshold (not the one for the ROC)

  //: constructor
  dborl_eval_sel_edge_linker_params(vcl_string algo_name) : dborl_algo_params(algo_name) 
  { 
    algo_abbreviation_ = "sel_eval";

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
    cem_output_extension_.set_values(param_list_, "io", "cem_output_extension", "contour output extension", ".cem", ".cem");
    ROC_output_extension_.set_values(param_list_, "io", "ROC_output_extension", "ROC output extension", ".roc_edg", ".roc_edg");

    output_dir_.set_values(param_list_,  "io",  "output_edge_folder",  "output folder to write edge map", "", 
      "/vision/projects/kimia/categorization/output/", 
      0,   // associate to 0th input object
      dborl_parameter_system_info::OUTPUT_FILE, "edge_map", dborl_parameter_type_info::FILEASSOC);

    // set default parameters
    save_edges_.set_values(param_list_, "io", "save_edges", "Save edges?", true, false);
    save_contours_.set_values(param_list_, "io", "save_contours", "Save contours?", true, false);
    save_to_object_folder_.set_values(param_list_, "io", "save_to_object_folder", "save to object folder?", false, false);

    // set edge detection parameters
    edge_det_type_.set_values(param_list_, "edge_det", "det_type", "edge detector: [1]=gen, [2]=TO", 2, 2); 
    edge_det_sigma_.set_values(param_list_, "edge_det", "det_sigma", "edge detector sigma", 1.0, 1.0);
    edge_det_thresh_.set_values(param_list_, "edge_det", "det_thresh", "edge response threshold", 2.0, 1.0);
    edge_det_N_.set_values(param_list_, "edge_det", "det_interp_N", "Interpolation depth", 1, 1);

    // set edge linking parameters
    curve_model_type_.set_values(param_list_, "edge_link", "curve_model", "Curve Model: [0] Linear, [1] CC, [2] CC3d, [3] ES", 2, 2);
    nrad_.set_values(param_list_, "edge_link", "nrad", "Radius of the Neighborhood", 4.0, 4.0);
    dt_.set_values(param_list_, "edge_link", "dt", "Orientation Uncertainty", 15,15);
    dx_.set_values(param_list_, "edge_link", "dx", "Position Uncertainty", 0.2, 0.2);
    badap_uncer_.set_values(param_list_, "edge_link", "adap_uncer", "Use the Uncertainty from edges?", false, false);
    token_len_.set_values(param_list_, "edge_link", "token_len", "Length of Edgel Token", 1.0, 1.0);
    max_k_.set_values(param_list_, "edge_link", "max_k", "Max k", 0.3, 0.3);
    max_gamma_.set_values(param_list_, "edge_link", "max_g", "Max Gamma", 0.01, 0.01);
    cvlet_type_.set_values(param_list_, "edge_link", "cvlet_type", "Grouping: [0] AC, [1] AC-BD, [2] BD, [3] ENO", 0, 0);
 
    hybrid_method_.set_values(param_list_, "edge_link", "hybrid_method", "Hybrid method: [0] No, [1] Filter Gen Only, [2] Filter Gen + Some", 0, 0);
 
    len_thresh_.set_values(param_list_, "edge_link", "prune_len_thresh", "Contour length threshold", 10.0, 10.0);
    
  }

};

#endif  //_dborl_eval_sel_edge_linker_params_h
