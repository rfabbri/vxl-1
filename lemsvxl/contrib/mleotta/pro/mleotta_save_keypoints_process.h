// This is contrib/mleotta/pro/mleotta_save_keypoints_process.h

#ifndef mleotta_save_keypoints_process_h_
#define mleotta_save_keypoints_process_h_

//:
// \file
// \brief A process for saving keypoints into a binary file (no repository)
// \author Matt Leotta
// \date 01/21/08
//
// \verbatim
//  Modifications
// \endverbatim


#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>
#include <list>
#include <dbdet/dbdet_keypoint_sptr.h>

class mleotta_save_keypoints_process : public bpro1_process
{
 public:
  
  mleotta_save_keypoints_process();
 ~mleotta_save_keypoints_process();

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
  std::vector<std::vector< dbdet_keypoint_sptr > > all_keypoints_;
};

#endif // mleotta_save_keypoints_process
