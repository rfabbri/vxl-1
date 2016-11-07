// This is brcv/seg/dbdet/pro/dbdet_third_order_edge_detector_process.h
#ifndef dbdet_third_order_edge_detector_vxl_process_h_
#define dbdet_third_order_edge_detector_vxl_process_h_

//:
// \file
// \brief A process to compute a subpixel edges using the third order edge detector
// \author Amir Tamrakar
// \date 11/10/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: Third order edge detector
class dbdet_third_order_edge_detector_vxl_process : public bpro1_process 
{
public:

  dbdet_third_order_edge_detector_vxl_process();
  virtual ~dbdet_third_order_edge_detector_vxl_process();

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
