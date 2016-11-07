// This is contrib/mleotta/modrec/pro/modrec_depthmap_process.h
#ifndef modrec_depthmap_process_h_
#define modrec_depthmap_process_h_

//:
// \file
// \brief Process that renders a depth map of a 3D model
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 01/05/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

#include <vil/vil_image_view.h>

//: Process that renders a depth map of a 3D model
class modrec_depthmap_process : public bpro1_process {

public:

  modrec_depthmap_process();
  virtual ~modrec_depthmap_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  //: Returns a vector of strings with suggested names for output classes
  vcl_vector< vcl_string > suggest_output_names();

  bool execute();
  bool finish();

};

#endif
