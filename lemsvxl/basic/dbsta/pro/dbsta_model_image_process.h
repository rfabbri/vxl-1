// This is brcv/seg/dbsta/pro/dbsta_model_image_process.h
#ifndef dbsta_model_image_process_h_
#define dbsta_model_image_process_h_

//:
// \file
// \brief Process that builds a statistical model of an image
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 8/18/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

//: Derived video process class for statistical image modeling
class dbsta_model_image_process : public bpro1_process {

public:

  dbsta_model_image_process();
  virtual ~dbsta_model_image_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name();

  int input_frames();
  int output_frames();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

};

#endif
