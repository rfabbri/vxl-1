//---------------------------------------------------------------------
// This is brcv/rec/dbcvr/pro/dbcvr_mutual_info_process.h
//:
// \file
// \brief process to find mutual information between regions of interest in input images
//
// \author
//  O.C. Ozcanli - June 08, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dbcvr_mutual_info_process_h_
#define dbcvr_mutual_info_process_h_

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <dbcvr/dbcvr_cvmatch.h>
#include <dbcvr/dbcvr_cvmatch_sptr.h>
#include <dbcvr/dbcvr_clsd_cvmatch.h>
#include <dbcvr/dbcvr_clsd_cvmatch_sptr.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbcvr/pro/dbcvr_curvematch_storage.h>
#include <dbcvr/pro/dbcvr_curvematch_storage_sptr.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

class dbcvr_mutual_info_process : public bpro1_process 
{
public:
  dbcvr_mutual_info_process();
  virtual ~dbcvr_mutual_info_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Mutual Info Matching";
  }

  vcl_vector< vcl_string > get_input_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
    to_return.push_back( "image" );
    to_return.push_back( "vsol2D" );
    to_return.push_back( "image" );
    return to_return;
  }
  vcl_vector< vcl_string > get_output_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    return to_return;
  }

  int input_frames() {
    return 1;
  }
  int output_frames() {
    return 1;
  }

  bool execute();
  bool finish() {
    return true;
  }

  float get_total_info()        { return total_info_; }
  float get_int_mutual_info()   { return int_mutual_info_; }
  float get_grad_mutual_info()  { return grad_mutual_info_; }
  float get_color_mutual_info() { return color_mutual_info_; }

protected:
  float total_info_;
  float int_mutual_info_;
  float grad_mutual_info_;
  float color_mutual_info_;

private:
  

};

#endif
