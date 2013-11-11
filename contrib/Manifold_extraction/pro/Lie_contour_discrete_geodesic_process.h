/*************************************************************************
 *    NAME: pradeep
 *    FILE: Lie_contour_discrete_geodesic_process.h
 *    DATE: 17 July 2007
 // brief:  process to find out the geodesics in the discrete case (the case in which 
 we deal with the discrete samples obtained from the tangent space of the intrinsic mean)
 *************************************************************************/

#ifndef Lie_contour_discrete_geodesic_process_header
#define Lie_contour_discrete_geodesic_process_header

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

class Lie_contour_discrete_geodesic_process : public bpro1_process 
{
protected:

  void loadCON(vcl_string fileName, vcl_vector<vsol_point_2d_sptr> &points);
  double compute_lie_cost(vcl_vector<vsol_point_2d_sptr> curve1_samples,vcl_vector<vsol_point_2d_sptr> curve2_samples );
  void angles_scales(vcl_vector<vsol_point_2d_sptr> curve1,vcl_vector<vsol_point_2d_sptr> curve2,
                     vcl_vector<double> &angles,vcl_vector<double> &scales);

public:
  Lie_contour_discrete_geodesic_process();
  virtual ~Lie_contour_discrete_geodesic_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "lie contour discrete geodesics";
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

