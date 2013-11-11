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

#ifndef vox_third_order_edge_det_params_h_
#define vox_third_order_edge_det_params_h_

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm 
//  are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class vox_third_order_edge_det_params : public dborl_algo_params
{
public:

  //: Constructor
    vox_third_order_edge_det_params(vcl_string algo_name);

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

  //: extension of edge file if saving
  dborl_parameter<vcl_string > output_edgeorient_extension_;

  dborl_parameter<vcl_string> output_edgemap_extension_;

  dborl_parameter<vcl_string > output_cemv_tag_;

  //: extension of output file
  dborl_parameter<vcl_string> output_extension_;

  // if written to this folder as opposed to object folder then the shock graph 
  // gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_edge_link_folder_; 

  //: Prune contours after edge linking
  dborl_parameter<bool> prune_contours_;  

  dborl_parameter<vcl_string> pyramid_folder_;

  //: Tag for edge_detection
  vcl_string tag_gray_edge_detection_;

  //: Tag for edge_detection
  vcl_string tag_color_edge_detection_;

  //: Tag for edge_linking
  vcl_string tag_edge_linking_;

  //: Tag for extract_contours
  vcl_string tag_extract_contours_;

  //: Tag for prune_contours
  vcl_string tag_prune_contours_;

};

#endif  //_vox_third_order_edge_det_params_h
