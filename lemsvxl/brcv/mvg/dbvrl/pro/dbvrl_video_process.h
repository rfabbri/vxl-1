// This is brcv/mvg/dbvrl/pro/dbvrl_video_process.h
#ifndef dbvrl_video_process_h_
#define dbvrl_video_process_h_

//:
// \file
// \brief Process that registers successive frames of a video
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 4/26/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

//: Derived video process class for computing transformations
class dbvrl_video_process : public bpro1_process {

public:

  dbvrl_video_process();
  virtual ~dbvrl_video_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name();

  int input_frames();
  int output_frames();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

protected:
  bool first_frame_;

};

#endif //dbvrl_video_process_h_
