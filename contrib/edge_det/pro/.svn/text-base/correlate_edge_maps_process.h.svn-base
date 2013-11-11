// This is pro/correlate_edge_maps_process.h
#ifndef correlate_edge_maps_process_h_
#define correlate_edge_maps_process_h_

//:
// \file
// \brief A process to compute edge map correlations 
// \author Amir Tamrakar
// \date 12/14/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: 
class correlate_edge_maps_process : public bpro1_process 
{
public:

  correlate_edge_maps_process();
  virtual ~correlate_edge_maps_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

};

#endif
