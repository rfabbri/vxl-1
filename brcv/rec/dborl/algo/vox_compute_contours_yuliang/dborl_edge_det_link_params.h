// This is dborl/algo/edgeLinking/dborl_edge_det_link_params.h

//:
// \file
// \brief parameter set for combination of edge_detection & linking algorithm
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date June 05, 2009
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#ifndef dborl_edge_det_link_params_h_
#define dborl_edge_det_link_params_h_

#include <dborl/algo/dborl_algo_params.h>
#include <dbdet/pro/dbdet_convert_edgemap_to_image_process.h>

//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm 
//  are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class dborl_edge_det_link_params : public dborl_algo_params
{
public:

  //: Constructor
  dborl_edge_det_link_params(vcl_string algo_name);

  // MEMBER VARIABLES

  //: Save the edges as .edg file
  dborl_parameter<bool> save_edges_;

  //: Save the curvlets as .cvlet file
  dborl_parameter<bool> save_curvelets_;

  //: Save the output to the object folder
  dborl_parameter<bool> save_to_object_folder_;  

  //: Name of input object
  dborl_parameter<vcl_string> input_object_name_;
  
  //: passes the folder of the input object
  dborl_parameter<vcl_string> input_object_dir_;    

  //: extension of the input image
  dborl_parameter<vcl_string> input_extension_;       

  //: extension of edge file if saving
  dborl_parameter<vcl_string> edge_extension_;

  //: extension of output file
  dborl_parameter<vcl_string> output_extension_;

  //: extension of output image
  dborl_parameter<vcl_string> output_image_extension_;

  // if written to this folder as opposed to object folder then the shock graph 
  // gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_edge_link_folder_; 

  //: Use existing edge file
  // An existing edge file might exist go ahead and use it
  dborl_parameter<bool> use_existing_edges_;

  //: Use existing cem file
  // An existing cem file might exist go ahead and use it
  dborl_parameter<bool> use_existing_cem_;

  //: Prune contours after edge linking
  dborl_parameter<bool> prune_contours_;  

  //: Trace contours , edge detection and linking ignored
  dborl_parameter<bool> trace_contours_;  

  //: Edge Linking Method to use
  dborl_parameter<vcl_string> edge_linking_method_;

  //: Convert to binary map
  dborl_parameter<bool> convert_edgemap_to_image_; 
 
  //: Tag for edge_detection
  vcl_string tag_gray_edge_detection_;

  //: Tag for edge_detection
  vcl_string tag_color_edge_detection_;

  //: Tag for symbolic edge_linking
  vcl_string tag_edge_linking_;

  //: Tag for extract_contours
  vcl_string tag_extract_contours_;

  //: Tag for prune_contours
  vcl_string tag_prune_contours_;

  //: Tag for contour tracing
  vcl_string tag_contour_tracing_;

  //: Tag for convert to binary map
  vcl_string tag_convert_edgemap_to_image_;

  //: Tag for generic edge_linking
  vcl_string tag_gen_linking_;

};

#endif  //_dborl_edge_det_link_params_h
