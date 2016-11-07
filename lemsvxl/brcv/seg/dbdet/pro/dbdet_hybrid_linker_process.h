// This is brcv/seg/dbdet/pro/dbdet_hybrid_linker_process.h
#ifndef dbdet_hybrid_linker_process_h_
#define dbdet_hybrid_linker_process_h_

//:
// \file
// \brief Process to perform edge linking by combining greedy and curvelet based method
// \author Amir Tamrakar
// \date 05/02/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: This process produces a contour fragment graph from an edge map
class dbdet_hybrid_linker_process : public bpro1_process 
{
public:

  dbdet_hybrid_linker_process();
  virtual ~dbdet_hybrid_linker_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

  void get_parameters();

protected:
  //various parameters
  double nrad, dx, dt;
  bool badap_uncer;
  unsigned curve_model_type;
  double max_k, max_gamma;
  unsigned grouping_algo;
  
  unsigned cvlet_type;
  bool bCentered_grouping;
  bool bBidirectional_grouping;

  unsigned max_size_to_group;
  bool b_use_all_cvlets;
  
  unsigned linkgraph_algo;
  unsigned min_size_to_link;
  unsigned num_link_iters;

  unsigned cvlet_form;
  unsigned post_process; 

};

#endif
