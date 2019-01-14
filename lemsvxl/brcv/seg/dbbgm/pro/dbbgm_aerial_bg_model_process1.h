// This is pro/dbbgm_aerial_bg_model_process1.h
#ifndef dbbgm_aerial_bg_model_process1_h_
#define dbbgm_aerial_bg_model_process1_h_

//:
// \file
// \brief 
// \author Vishal Jain(vj@lems.brown.edu)
// \date 11/11/03
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>
#include <bbgm/bbgm_image_sptr.h>

//: Derived video process class for computing frame difference
class dbbgm_aerial_bg_model_process1 : public bpro1_process {

public:

  dbbgm_aerial_bg_model_process1();
  ~dbbgm_aerial_bg_model_process1();

  std::string name();

  //: Clone the process
  virtual bpro1_process* clone() const;

  int input_frames();
  int output_frames();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

   
private:
    bbgm_image_sptr model_;

};

#endif
