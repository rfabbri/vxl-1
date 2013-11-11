// This is brcv/seg/dbdet/pro/dbdet_sel_extract_contours_process.h
#ifndef dbdet_sel_extract_contours_process_h_
#define dbdet_sel_extract_contours_process_h_

//:
// \file
// \brief Process to extract contours from a sel_storage class
// \author Amir Tamrakar
// \date 04/03/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

class dbdet_sel_extract_contours_process : public bpro1_process 
{
public:

  dbdet_sel_extract_contours_process();
  virtual ~dbdet_sel_extract_contours_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

protected:
  //various parameters
  unsigned len_thresh; ///< length threshold

  bool apply_strength_thresh; ///< apply avg. strength threshold flag
  double strength_thresh;     ///< avg. strength threshold
  bool apply_contrast_thresh; ///< apply mean contrast threshold flag
  double contrast_thresh;     ///< mean contrast threshold
  bool apply_adap_thresh;     ///< apply adaptive threshold flag
  double adap_thresh_fac;     ///< adaptive threshold factor
  bool apply_d2f_thresh;      ///< apply d2f threshold
  double d2f_thresh;          ///< d2f threshold

  bool smooth_con;     ///< smooth contour flag
  float psi;           ///< smoothing parameter
  unsigned num_times;  ///< # times to smooth
  bool apply_k_thresh; ///< apply curvature threshold
  float k_thresh;      ///< curvature threshold for breaking contours

};

#endif
