/*************************************************************************
 *    NAME: pradeep yarlagadda
 *    FILE: Lie_cost_geodesics_process.h
 //   this process returns the Lie cost associated for matching a pair of curves 
 //   which are already partwise aligned 
 *************************************************************************/

#ifndef Lie_cost_geodesics_process_header
#define Lie_cost_geodesics_process_header

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>


#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

class Lie_cost_geodesics_process : public bpro1_process 
{
protected:
 double compute_lie_cost( vcl_vector<vsol_point_2d_sptr> curve1_samples, vcl_vector<vsol_point_2d_sptr> curve2_samples);
 
 double compute_kimia_cost( vcl_vector<vsol_point_2d_sptr> curve1_samples, vcl_vector<vsol_point_2d_sptr> curve2_samples,
                                                      vcl_vector<double> tangent_angle_c1,vcl_vector<double> tangent_angle_c2);

public:
  Lie_cost_geodesics_process();
  virtual ~Lie_cost_geodesics_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Lie cost geodesics";
  }

  

  vcl_vector< vcl_string > get_input_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
    to_return.push_back( "vsol2D" );
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

