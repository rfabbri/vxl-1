#ifndef mw_extend_corresp_process_h_
#define mw_extend_corresp_process_h_

//:
// \file
// \brief run mw_discrete_corresp_algo::extend in two frames.
// \author Ricardo Fabbri
// \date Sun Oct 25 18:45:54 PDT 2009
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <mw/mw_discrete_corresp.h>

//: Given:
// - two frames, 0 and 1, each having two vsol storages, a0, b0, and a1, b1
// - a correspondence acorr from a0 to a1
// This process creates a new correspondence between b0 and b1 which is the
// correspondence induced by acorr on the common elements between (a0,b0) and
// (a1, b1).
//
// The frames 0 and 1 above are  the previous (0) and current (1) frames.
class mw_extend_corresp_process : public bpro1_process {
public:

  mw_extend_corresp_process();
  virtual ~mw_extend_corresp_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

private:
  vcl_vector< vsol_polyline_2d_sptr > a0_;
  vcl_vector< vsol_polyline_2d_sptr > a1_;
  vcl_vector< vsol_polyline_2d_sptr > b0_;
  vcl_vector< vsol_polyline_2d_sptr > b1_;
  mw_discrete_corresp *acorr_;
  bool get_vsols(unsigned v, unsigned input_id, vcl_vector< vsol_polyline_2d_sptr > *pcurves);
};

#endif
