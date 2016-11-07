// This is brl/vidpro/process/vidpro_grey_image_process.h
#ifndef vidpro_grey_image_process_h_
#define vidpro_grey_image_process_h_

//:
// \file
// \brief Process that converts an image into grey-scale
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 12/16/03
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro/bpro_process.h>

class vidpro_grey_image_process : public bpro_process {

public:

  vidpro_grey_image_process();
  virtual ~vidpro_grey_image_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
  
protected:

private:

};

#endif // vidpro_grey_image_process_h_
