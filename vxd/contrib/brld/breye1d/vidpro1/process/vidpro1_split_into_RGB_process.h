// This is brl/vidpro1/process/vidpro1_split_into_RGB_process.h
#ifndef vidpro1_split_into_RGB_process_h_
#define vidpro1_split_into_RGB_process_h_

//:
// \file
// \brief Process that splits an RGB image into separate color channel images
// \author Based on original code by Amir Tamrakar
// \date 01/22/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

//: Process that splits an RGB image into separate color channel images
class vidpro1_split_into_RGB_process : public bpro1_process {

public:
  //: Constructor
  vidpro1_split_into_RGB_process();
  //: Destructor
  virtual ~vidpro1_split_into_RGB_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

};

#endif
