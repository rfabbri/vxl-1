/*************************************************************************
 *    NAME: pradeep
 *    FILE: Lie_spoke_geodesic_process.h
 *    DATE: 06 Aug 2007
 // brief:  process to transform all the shapes into the tangent space of intrinsic mean shape
 // which would then be used to find out principal geodesics
 *************************************************************************/

#ifndef Lie_spoke_geodesic_process_header
#define Lie_spoke_geodesic_process_header

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <dbcvr/dbcvr_cvmatch.h>
#include <dbcvr/dbcvr_cvmatch_sptr.h>

#include <dbcvr/pro/dbcvr_curvematch_storage.h>
#include <dbcvr/pro/dbcvr_curvematch_storage_sptr.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

class Lie_spoke_geodesic_process : public bpro1_process 
{
protected:

public:
  Lie_spoke_geodesic_process();
  virtual ~Lie_spoke_geodesic_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "lie spoke geodesics";
  }

  vcl_vector< vcl_string > get_input_type() {
    vcl_vector< vcl_string > to_return;
    // to_return.push_back( "vsol2D" );
    return to_return;
  }
  vcl_vector< vcl_string > get_output_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
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

};

#endif

