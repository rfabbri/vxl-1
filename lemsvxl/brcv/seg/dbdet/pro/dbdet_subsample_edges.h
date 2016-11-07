// This is brcv/seg/dbdet/pro/dbdet_subsample_edges.h
#ifndef dbdet_subsample_edges_h_
#define dbdet_subsample_edges_h_

//:
// \file
// \brief Process to perform greedy generic edge linking
// \author Amir Tamrakar
// \date 03/29/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: This process traces contours of binary regions in an image to produce subpixel contours
class dbdet_subsample_edges : public bpro1_process 
{
public:

  dbdet_subsample_edges();
  virtual ~dbdet_subsample_edges();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

  void get_parameters();

protected:
  //various parameters
  unsigned subsampling_algo;

  unsigned SCF_len;
  double linefit_rms;
  bool bremove_short_frags;

};

#endif
