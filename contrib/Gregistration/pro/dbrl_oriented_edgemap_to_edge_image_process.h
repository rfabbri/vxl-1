//This is contrib/Gregistration/pro/dbrl_oriented_edgemap_to_edge_image_process.h
#ifndef dbrl_oriented_edgemap_to_edge_image_process_h_
#define dbrl_oriented_edgemap_to_edge_image_process_h_

//:
// \file
// \brief A process to convert an edgemap to an edge image where edge information is stored on rgb channels
// \author Anil Usumezbas (anilusumezbas@gmail.com)
// \date Sep 12, 2011
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: A process to convert an edgemap to an edge image where edge information is stored on rgb channels
class dbrl_oriented_edgemap_to_edge_image_process : public bpro1_process
{
 public:

  dbrl_oriented_edgemap_to_edge_image_process();
  virtual ~dbrl_oriented_edgemap_to_edge_image_process();

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
