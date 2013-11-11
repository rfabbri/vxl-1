// This is brcv/seg/dbdet/pro/dbdet_compass_edge_detector_process.h
#ifndef dbdet_compass_edge_detector_process_h_
#define dbdet_compass_edge_detector_process_h_

//:
// \file
// \brief A process to detect edges using Ruzon and Tomasi's Compass operator
// \author Amir Tamrakar
// \date 08/27/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: This process allows for a choice of signature computation method
//  as well as the choice of histogram distances 
class dbdet_compass_edge_detector_process : public bpro1_process 
{
public:

  dbdet_compass_edge_detector_process();
  virtual ~dbdet_compass_edge_detector_process();

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
