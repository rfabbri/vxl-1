// This is brcv/seg/dbdet/pro/dbdet_prune_edgemap_process.h
#ifndef dbdet_prune_edgemap_process_h_
#define dbdet_prune_edgemap_process_h_

//:
// \file
// \brief A process to prune an edgemap based on various edge properties 
// \author Amir Tamrakar
// \date 11/01/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: Third order edge detector
class dbdet_prune_edgemap_process : public bpro1_process 
{
public:

  dbdet_prune_edgemap_process();
  virtual ~dbdet_prune_edgemap_process();

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
