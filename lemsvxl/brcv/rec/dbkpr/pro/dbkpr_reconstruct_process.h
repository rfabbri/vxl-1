// This is brcv/seg/dbkpr/pro/dbkpr_reconstruct_process.h
#ifndef dbkpr_reconstruct_process_h_
#define dbkpr_reconstruct_process_h_

//:
// \file
// \brief Process that reconstructs the matched keypoints
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 4/20/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Derived video process class for reconstructing matched keypoints
class dbkpr_reconstruct_process : public bpro1_process {

public:

  dbkpr_reconstruct_process();
  virtual ~dbkpr_reconstruct_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

};

#endif
