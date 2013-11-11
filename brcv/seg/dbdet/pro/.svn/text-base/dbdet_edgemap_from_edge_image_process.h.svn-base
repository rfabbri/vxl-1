// This is brcv/seg/dbdet/pro/dbdet_edgemap_from_edge_image_process.h
#ifndef dbdet_edgemap_from_edge_image_process_h_
#define dbdet_edgemap_from_edge_image_process_h_

//:
// \file
// \brief A process to convert an edge image into an edge map
// \author Amir Tamrakar
// \date 11/10/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: Given a grayscale image, create an edgel for each pixel whose value equals a supplied parameter.
// The generated edgel will have orientation 0 degrees.
class dbdet_edgemap_from_edge_image_process : public bpro1_process 
{
public:

  dbdet_edgemap_from_edge_image_process();
  virtual ~dbdet_edgemap_from_edge_image_process();

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
