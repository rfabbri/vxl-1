// This is brcv/seg/dbkpr/pro/dbdet_combine_curves_process.h
#ifndef dbdet_combine_curves_process_h_
#define dbdet_combine_curves_process_h_

//:
// \file
// \brief Combines contours from multiple frames
// \author Ricardo Fabbri
// \date 4/23/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Combines contours from multiple frames. Outputs a vsol in each frame that is the union of all
// vsols in all frames. Before running this process, the user might want to run
// dbdet_prune_curves_process in each frame.
// 
class dbdet_combine_curves_process : public bpro1_process {

public:

  dbdet_combine_curves_process();
  virtual ~dbdet_combine_curves_process();

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
