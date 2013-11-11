// This is dbdet_compute_linked_curves_process.h
#ifndef dbdet_compute_linked_curves_process_h
#define dbdet_compute_linked_curves_process_h
//:
//\file
//\brief Process to perform edge detection + linking + smoothing on the fly
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 02/02/2009 11:40:51 PM EST
//

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

// Conveniently combines the 3 processes of 
//  - third_order_edge_detector
//  - symbolic edge linker
//  - extract contours
//
// It uses the parameters that are current best practice in LEMS.
//
// \todo parameter dialogs instead of hardcoded.
class dbdet_compute_linked_curves_process : public bpro1_process 
{
public:

  dbdet_compute_linked_curves_process();
  virtual ~dbdet_compute_linked_curves_process();

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



#endif // dbdet_compute_linked_curves_process_h

