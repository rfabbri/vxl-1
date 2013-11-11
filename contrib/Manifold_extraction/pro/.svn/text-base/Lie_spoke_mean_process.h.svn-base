/*************************************************************************
 *    NAME: pradeep
 *    FILE: Lie_spoke_mean_process.h
 *    DATE: 04 July 2007
 // brief:  process to find out the intrinsic mean shape (in Lie sense) given a set of 
 //         contour files having the same number of spokes
 *************************************************************************/

#ifndef Lie_spoke_mean_process_header
#define Lie_spoke_mean_process_header

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

//value in (-Pi,Pi]
//double spoke_curve_fixAngleMPiPi (double a);

class Lie_spoke_mean_process : public bpro1_process 
{
protected:


  double compute_lie_cost(vcl_vector<vsol_point_2d_sptr> curve1_samples,vcl_vector<vsol_point_2d_sptr> curve2_samples );
  void angles_scales(vcl_vector<vsol_point_2d_sptr> curve1,vcl_vector<vsol_point_2d_sptr> curve2,
                     vcl_vector<double> &angles,vcl_vector<double> &scales);
  vcl_vector<vsol_point_2d_sptr> transform_shape(vcl_vector<vsol_point_2d_sptr> curve1,
                                             vcl_vector<double> angles,vcl_vector<double> scales);
  /*void generate_values_along_geodesic(vcl_vector<double> angles,vcl_vector<double> scales,vcl_vector<double> &sample_angles,
                                      vcl_vector<double> &sample_scales,double t);*/
  vcl_vector<vsol_point_2d_sptr> closed_articulated_structure(vcl_vector<vsol_point_2d_sptr> final_points);
  void save_shape(vcl_vector<vsol_point_2d_sptr> new_shape,vcl_string new_shapes_path,unsigned int i);
  void save_shape_as_image(vcl_vector<vsol_point_2d_sptr> new_shape,vcl_string new_shapes_path,
                                                                 unsigned int i);
  vsol_point_2d_sptr compute_centroid(vcl_vector<vsol_point_2d_sptr> contour);
void get_spoke_scales_angles(vcl_vector<vsol_point_2d_sptr> contour,vcl_vector<vsol_point_2d_sptr> ref_contour,
                             vcl_vector<double> &scales,vcl_vector<double> &angles);
vcl_vector<vsol_point_2d_sptr> generate_spoke_configuration(vcl_vector<vsol_point_2d_sptr> contour);
void find_mean_scales_angles(vcl_vector<vcl_vector<double> > scales_vec,vcl_vector<vcl_vector<double> > angles_vec,
                             vcl_vector<double> &mean_scales,vcl_vector<double> &mean_angles);

 vcl_vector<vsol_point_2d_sptr> transform_spoke_shape(vcl_vector<vsol_point_2d_sptr> contour,vcl_vector<double> mean_scales,
                                                      vcl_vector<double> mean_angles);

  vcl_vector<vsol_point_2d_sptr> find_mean_shape(vcl_vector<vcl_vector<vsol_point_2d_sptr> > given_shapes,
                                                                        vcl_string file_name);

public:
  Lie_spoke_mean_process();
  virtual ~Lie_spoke_mean_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "lie spoke mean";
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

