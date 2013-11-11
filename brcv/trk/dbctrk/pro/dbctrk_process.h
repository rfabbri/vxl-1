/**************************************************************/
/*  Name: Vishal Jain (vj)
//  File: dbctrk_process.h
//  Asgn: bvis
//  Date: Mon Jul 21 16:01:53 EDT 2003
***************************************************************/

#ifndef dbctrk_process_header
#define dbctrk_process_header

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <dbctrk/pro/dbctrk_storage.h>
#include <dbctrk/pro/dbctrk_storage_sptr.h>

#include <vil1/vil1_vil.h>
#include <vil/vil_load.h>
#include <vil1/vil1_rgb_byte.h>

#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/vil_copy.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil/vil_convert.h>
#include <sdet/sdet_harris_detector_params.h>
#include <vcl_vector.h>
#include <vsol/vsol_point_2d.h>
#include <vil1/vil1_memory_image_of.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>
#include <dbctrk/dbctrk_curve_tracking.h>

class dbctrk_process : public bpro1_process {

public:

  dbctrk_process();
  ~dbctrk_process();

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
  sdet_detector_params dp;
  dbctrk_curve_tracking_params tp;
  dbctrk_curve_tracking * tracker_;
private:

};

#endif
