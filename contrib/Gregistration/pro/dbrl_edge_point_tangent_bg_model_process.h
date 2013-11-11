// This is pro/dbrl_edge_point_tangent_bg_model_process.h
#ifndef dbrl_edge_point_tangent_bg_model_process_h_
#define dbrl_edge_point_tangent_bg_model_process_h_

//:
// \file
// \brief 
// \author Vishal Jain(vj@lems.brown.edu)
// \date 08/30/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <dbbgm/dbbgm_distribution_image_sptr.h>

//: Derived video process class for computing frame difference
class dbrl_edge_point_tangent_bg_model_process : public bpro1_process {

public:

  dbrl_edge_point_tangent_bg_model_process();
  ~dbrl_edge_point_tangent_bg_model_process();

  vcl_string name();

  //: Clone the process
  virtual bpro1_process* clone() const;

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

   
private:
    dbbgm_distribution_image_sptr model_;

};

#endif
