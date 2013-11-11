// This is brcv/seg/dbdet/pro/dbdet_convert_edgemap_to_image_process.h
#ifndef dbdet_convert_edgemap_to_image_process_h_
#define dbdet_convert_edgemap_to_image_process_h_

//:
// \file
// \brief A process to convert an edgemap to a binary image of the edges
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date May 30, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: A process to convert an edgemap to a binary image of the edges
class dbdet_convert_edgemap_to_image_process : public bpro1_process 
{
public:

  dbdet_convert_edgemap_to_image_process();
  virtual ~dbdet_convert_edgemap_to_image_process();

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
