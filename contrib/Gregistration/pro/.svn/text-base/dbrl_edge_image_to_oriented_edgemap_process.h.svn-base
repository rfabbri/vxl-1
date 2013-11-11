//This is contrib/Gregistration/pro/dbrl_edge_image_to_oriented_edgemap_process.h
#ifndef dbrl_edge_image_to_oriented_edgemap_process_h_
#define dbrl_edge_image_to_oriented_edgemap_process_h_

//:
// \file
// \brief A process to convert an rgb edge image to an edge map
// \author Anil Usumezbas (anilusumezbas@gmail.com)
// \date Sep 12, 2011
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: A process to convert an rgb edge image to an edge map
class dbrl_edge_image_to_oriented_edgemap_process : public bpro1_process
{
 public:

  dbrl_edge_image_to_oriented_edgemap_process();
  virtual ~dbrl_edge_image_to_oriented_edgemap_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector<vcl_string> get_input_type();
  vcl_vector<vcl_string> get_output_type();

  bool execute();
  bool finish();
};

#endif
