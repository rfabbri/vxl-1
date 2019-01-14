// This is contrib/biotree/biocts/pro/biocts_process.h

#ifndef biocts_process_h_
#define biocts_process_h_

//:
// \file
// \brief A process for computing the CT simulation from a volumetric data file
// \author Rahul Gautam (corresponding author) & Amir Tamrakar
// \date 07/14/04
//
// \verbatim
//  Modifications
// \endverbatim


#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>
#include <biotree/biocts/biocts_ctsim.h>

class biocts_process : public bpro1_process
{
 public:
  
  biocts_process();
 ~biocts_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  void clear_output();

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();
 
 protected:

 int num_frames_; //number of views in the CT volume below

};

#endif // biocts_process_h_
