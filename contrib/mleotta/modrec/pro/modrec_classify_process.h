// This is contrib/mleotta/modrec/pro/modrec_classify_process.h
#ifndef modrec_classify_process_h_
#define modrec_classify_process_h_

//:
// \file
// \brief Process that classifies using 3D vehicle models
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 12/14/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

#include <vil/vil_image_view.h>
#include <imesh/imesh_mesh.h>

//: Process that aligns a 3D model to a background model
class modrec_classify_process : public bpro1_process {

public:

  modrec_classify_process();
  virtual ~modrec_classify_process();

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

private:
  vcl_vector<vil_image_view<float> > error_images_;
  vcl_vector<vcl_string>  mesh_names_;
  vcl_vector<imesh_mesh> mesh_;
};

#endif
