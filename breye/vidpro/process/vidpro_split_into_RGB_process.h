// This is brl/vidpro/process/vidpro_split_into_RGB_process.h
#ifndef vidpro_split_into_RGB_process_h_
#define vidpro_split_into_RGB_process_h_

//:
// \file
// \brief Process that splits an RGB image into separate color channel images
// \author Amir Tamrakar
// \date 01/22/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro/bpro_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Process that splits an RGB image into separate color channel images
class vidpro_split_into_RGB_process : public bpro_process {

public:
  //: Constructor
  vidpro_split_into_RGB_process();
  //: Destructor
  virtual ~vidpro_split_into_RGB_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  //: Returns the name of this process
  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

};

#endif
