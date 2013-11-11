// This is rec/dborl/algo/mpi/symseg/dborl_sks_match_shape_params.h
#ifndef dborl_sks_match_shape_params_h
#define dborl_sks_match_shape_params_h


//:
// \file
// \brief The parameter class for shape detection in images
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Jan 4, 2008
//      
// \verbatim
//   Modifications
//  
// \endverbatim



#include <bxml/bxml_document.h>
#include <dbskr/dbskr_sm_cor.h>
#include "dborl_sks_match_shape_params.h"
#include <dbsks/dbsks_circ_arc_grid.h>
#include <dbsks/dbsks_shapelet_grid.h>

#include <bpro1/bpro1_parameters_sptr.h>

class dborl_sks_arc_image_cost_params
{ 
public:

  ////
  //float edge_strength_threshold_; //: " , "-edge_threshold", 127.0f) ||
  //float chamfer_sigma_;           //: " , "-chamfer_sigma", 4.0f) ||
  //float arc_sampling_rate_ds_;    // = " , "-ds", 3.0f) ||
  //  
  //bool Use_image_center_;           //?: " , "-use_image_center", false ) ||
  //float center_x_;                  //: " , "-center_x", 256.0f ) ||
  //float center_y_;   //: " , "-center_y", 256.0f ) ||
  //  
  //float step_x_;  //: " , "-step_x", 8.0f ) ||
  //float step_y_;  //: " , "-step_y", 8.0f ) ||
  //  
  //bool Use_image_size_; //?: " , "-use_image_size", false ) ||
  //int half_num_x_; //: " , "-half_num_x", int(32) ) ||
  //int half_num_y_; //: " , "-half_num_y", int(32) ) ||
  //  
  //int num_theta_; //: " , "-num_theta", int(16) ) ||
  //  
  //float min_chord_; //: " , "-min_chord", 8.0f ) ||
  //float step_chord_; //: " , "-step_chord", 8.0f ) ||
  //int  num_chord_; //: " , "-num_chord", int(20) ) ||
  //  
  //float step_height_; //: " , "-step_height", 4.0f ) ||
  //int half_num_height_; //: " , "-half_num_height", int(16) )

  float edge_threshold;
  dbsks_circ_arc_grid arc_grid;
  float ds_sampling;
  float chamfer_sigma;
};




//: 
class dborl_sks_match_shape_params
{ 
public:

  // Input / output parameters
  vcl_string file_list_;  // the list of input to execute this process on
  vcl_string file_dir_;  // the directory of input to execute this process on
  vcl_string out_dir_;  // the directory of input to execute this process on
  vcl_string model_file_;  // the shock model // same for all images so load just once
  vcl_string extension_;  // image/file extension

  // process parameters
  bpro1_parameters_sptr dbsks_compute_arc_cost_params_;
  bpro1_parameters_sptr dbsks_detect_shape_params_;
  bpro1_parameters_sptr dbsks_local_match_params_;

  bool parse_from_data(bxml_data_sptr root);
  static bxml_element *create_default_document_data();
  bxml_element *create_document_data();
};

#endif  //_dborl_sks_match_shape_params_h
