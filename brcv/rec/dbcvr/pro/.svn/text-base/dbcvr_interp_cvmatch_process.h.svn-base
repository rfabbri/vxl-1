//---------------------------------------------------------------------
// This is brcv/rec/dbcvr/pro/dbcvr_interp_cvmatch_process.h
//:
// \file
// \brief process to match to input curves using interpolated curve matching 
//        algorithm
//
// \author
//  O.C. Ozcanli - February 24, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#ifndef dbcvr_interp_cvmatch_process_header
#define dbcvr_interp_cvmatch_process_header

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>


#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbcvr/pro/dbcvr_curvematch_storage.h>
#include <dbcvr/pro/dbcvr_curvematch_storage_sptr.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

class dbcvr_interp_cvmatch_process : public bpro1_process 
{
public:
  dbcvr_interp_cvmatch_process();
  virtual ~dbcvr_interp_cvmatch_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Interpolated Curve Matching";
  }

  vcl_vector< vcl_string > get_input_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
    to_return.push_back( "vsol2D" );
    return to_return;
  }
  vcl_vector< vcl_string > get_output_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "curvematch" );
    return to_return;
  }

  int input_frames() {
    return 2;
  }
  int output_frames() {
    return 1;
  }

  bool execute();
  bool finish() {
    return true;
  }

  
protected:

private:

};

#endif
