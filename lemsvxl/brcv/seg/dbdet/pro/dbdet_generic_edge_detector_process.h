// This is brcv/seg/dbdet/pro/dbdet_generic_edge_detector_process.h
#ifndef dbdet_generic_edge_detector_process_h_
#define dbdet_generic_edge_detector_process_h_

//:
// \file
// \brief A process to compute a subpixel edges using image derivatives followed by NMS
// \author Amir Tamrakar
// \date 09/29/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: This process allows the user to select a gradient computation method and use
//  it to detect the location and orientation of subpixel edges
class dbdet_generic_edge_detector_process : public bpro1_process 
{
public:

  dbdet_generic_edge_detector_process();
  virtual ~dbdet_generic_edge_detector_process();

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
