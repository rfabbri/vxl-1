// This is brcv/seg/dbkpr/pro/dbkpr_frenet_reconstruct_process.h
#ifndef dbkpr_frenet_reconstruct_process_h_
#define dbkpr_frenet_reconstruct_process_h_

//:
// \file
// \brief Reconstructs matched keypoints using differential geometry
// \author Ricardo Fabbri
// \date 4/23/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Derived video process class for reconstructing matched keypoints
class dbkpr_frenet_reconstruct_process : public bpro1_process {

public:

  dbkpr_frenet_reconstruct_process();
  virtual ~dbkpr_frenet_reconstruct_process();

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
