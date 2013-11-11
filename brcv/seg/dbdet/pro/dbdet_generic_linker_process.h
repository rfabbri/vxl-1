// This is brcv/seg/dbdet/pro/dbdet_generic_linker_process.h
#ifndef dbdet_generic_linker_process_h_
#define dbdet_generic_linker_process_h_

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
class dbdet_generic_linker_process : public bpro1_process 
{
public:

  dbdet_generic_linker_process();
  virtual ~dbdet_generic_linker_process();

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
  double threshold;

  bool req_app_con;
  double app_threshold;

  bool req_ori_con;

  bool req_smooth_con;
  bool req_low_ks;

  bool bpost_process;
    unsigned win_len;
    double adap_thresh;
    double k_thresh;

  bool bprune_cons;
  unsigned len_thresh;
  double strength_thresh;
  double contrast_thresh;
  double adap_thresh_fac;
  double d2f_thresh;
  double avg_k_thresh;

  bool output_vsol;

};

#endif
