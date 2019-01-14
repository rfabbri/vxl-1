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


  double compute_lie_cost(std::vector<vsol_point_2d_sptr> curve1_samples,std::vector<vsol_point_2d_sptr> curve2_samples );
  void angles_scales(std::vector<vsol_point_2d_sptr> curve1,std::vector<vsol_point_2d_sptr> curve2,
                     std::vector<double> &angles,std::vector<double> &scales);
  std::vector<vsol_point_2d_sptr> transform_shape(std::vector<vsol_point_2d_sptr> curve1,
                                             std::vector<double> angles,std::vector<double> scales);
  /*void generate_values_along_geodesic(std::vector<double> angles,std::vector<double> scales,std::vector<double> &sample_angles,
                                      std::vector<double> &sample_scales,double t);*/
  std::vector<vsol_point_2d_sptr> closed_articulated_structure(std::vector<vsol_point_2d_sptr> final_points);
  void save_shape(std::vector<vsol_point_2d_sptr> new_shape,std::string new_shapes_path,unsigned int i);
  void save_shape_as_image(std::vector<vsol_point_2d_sptr> new_shape,std::string new_shapes_path,
                                                                 unsigned int i);
  vsol_point_2d_sptr compute_centroid(std::vector<vsol_point_2d_sptr> contour);
void get_spoke_scales_angles(std::vector<vsol_point_2d_sptr> contour,std::vector<vsol_point_2d_sptr> ref_contour,
                             std::vector<double> &scales,std::vector<double> &angles);
std::vector<vsol_point_2d_sptr> generate_spoke_configuration(std::vector<vsol_point_2d_sptr> contour);
void find_mean_scales_angles(std::vector<std::vector<double> > scales_vec,std::vector<std::vector<double> > angles_vec,
                             std::vector<double> &mean_scales,std::vector<double> &mean_angles);

 std::vector<vsol_point_2d_sptr> transform_spoke_shape(std::vector<vsol_point_2d_sptr> contour,std::vector<double> mean_scales,
                                                      std::vector<double> mean_angles);

  std::vector<vsol_point_2d_sptr> find_mean_shape(std::vector<std::vector<vsol_point_2d_sptr> > given_shapes,
                                                                        std::string file_name);

public:
  Lie_spoke_mean_process();
  virtual ~Lie_spoke_mean_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name() {
    return "lie spoke mean";
  }

  std::vector< std::string > get_input_type() {
    std::vector< std::string > to_return;
    // to_return.push_back( "vsol2D" );
    return to_return;
  }
  std::vector< std::string > get_output_type() {
    std::vector< std::string > to_return;
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

