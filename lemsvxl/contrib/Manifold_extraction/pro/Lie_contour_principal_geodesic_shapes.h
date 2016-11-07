/*************************************************************************
 *    NAME: pradeep
 *    FILE: Lie_contour_principal_geodesic_shapes.h
 *    DATE: 17 July 2007
 // brief:  process to generate shapes along the principal geodesics
 //         which were learnt from the training set
 *************************************************************************/

#ifndef Lie_contour_principal_geodesic_shapes_header
#define Lie_contour_principal_geodesic_shapes_header

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

class Lie_contour_principal_geodesic_shapes : public bpro1_process 
{

public:
  Lie_contour_principal_geodesic_shapes();
  virtual ~Lie_contour_principal_geodesic_shapes() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "lie contour principal geodesics";
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

