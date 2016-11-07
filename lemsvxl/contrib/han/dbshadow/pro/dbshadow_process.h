// This is brcv/mvg/dbecl/pro/dbshadow_process.h
#ifndef dbshadow_process_h_
#define dbshadow_process_h_

#define BIN_SIZE_ 100

//:
// \file
// \brief A process that converts vtol edges to dbecl episegs
// \author Dongjin Han (han@lems.brown.edu)
// \date 12/25/05
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro/bpro_process.h>
#include <vgl/algo/vgl_line_2d_regression.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_inverse.h>


#include <dbecl/dbecl_episeg.h>
#include <dbecl/dbecl_episeg_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <vgl/vgl_point_3d.h>

#include <bmrf/bmrf_network.h>
#include <bmrf/bmrf_curve_3d_builder.h>
#include <bmrf/bmrf_curvel_3d.h>
#include <bmrf/bmrf_curve_3d.h>
#include <sdet/sdet_detector_params.h>


#include <vnl/vnl_double_4.h>






class dbshadow_process : public bpro_process
{
private:
  float MARGIN_;
  int combination_;
  double epi_x_,epi_y_;
  double epi_x_WC_,epi_y_WC_;
  vcl_vector <vsol_digital_curve_2d_sptr> dcl_;
  vcl_vector <vcl_vector <vsol_digital_curve_2d_sptr> >dcl_edge_;

  vcl_vector <vgl_point_2d <double> > left_pts_;
  vcl_vector <vgl_point_2d <double> > right_pts_;

  vgl_line_2d <double> line_upper_;
  vgl_line_2d <double> line_lower_;
  double theta_pos_,theta_neg_;
 
  double first_minus_second_;
  bool display_0_frame_, display_n_frame_;

  vcl_vector < vcl_vector <dbecl_episeg_sptr> >episeglist;
  vcl_vector < vcl_vector <dbecl_episeg_sptr> >episeglist_edge_;
  vcl_vector <bool> angle_f_flag_,angle_r_flag_;

  double scale_;
  vcl_vector <vgl_point_3d <double> > pts_3d_;
  vcl_vector <vgl_point_3d <double> > pts_3d_front_,pts_3d_rear_;
  vnl_double_3 min_point_,max_point_;
  vnl_double_3x3 rot_ ;
  vnl_double_3x3 inv_rot_ ;
  vnl_double_4x4 bb_xform_;

 
  vcl_vector <vsol_polygon_2d_sptr > polys_;
  vcl_vector <vgl_polygon<double> > veh_cons_;
  vcl_vector <vgl_point_3d<double> > probe_list_;

  vnl_double_3 p0_,p7_;
  bool cluster_flag_;
  double mean_y_1_,mean_y_2_;
  vcl_vector <vgl_point_2d <double> >pts_upper_line_;
  vcl_vector <vgl_point_2d <double> >pts_lower_line_;
  double ang_;



  vnl_matrix <double> BB_RT_matt_;


  vnl_double_3x3 M_in_;
  double trans_dist;
  vnl_double_3 T, T_intpl;
  vnl_double_3x4 E1, E2, E3, E_intpl;

  // camera parameters..
  vcl_vector <vnl_double_3x4> PL_;
  vcl_vector<vnl_double_3x4 > Cam_List_;
  vcl_vector <vcl_vector <vnl_vector_fixed <double, 6 > > >blob_image_list_;
protected:
  int nframes_; 
  //: Camera intrinsic parameters
  vnl_double_3x3 K_;
  vnl_double_3x4 C_;
  vnl_double_3x4 cam_;
  //: Map from frame numbers to cameras
  //vcl_map<int,vnl_double_3x4> cameras_;
  vcl_vector<vnl_double_3x4> cameras_;

  vnl_vector <double> tnew_;
  sdet_detector_params dp;
public:

  dbshadow_process();
  ~dbshadow_process();

  dbshadow_process(const dbshadow_process& other);

  //: Clone the process
  virtual bpro_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();


  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();
  bool execute();
  bool finish();


  bool initialize_camera_N_string_scan();
  //void initialize_camera_3_string_scan();
  void map3d_2d();
  void probe_plane();
  void probe_bottom(vnl_double_4 s0, vnl_double_4 e0);
  void probe_front_plane(vnl_double_4 s0, vnl_double_4 e0);
  void probe_windshield(vnl_double_4 s0, vnl_double_4 e0);
  double count_edge(vnl_double_4 s0, vnl_double_4 e0);
  double shadow_histo(vnl_double_4 s0, vnl_double_4 e0);
  void shadow_histo();

};

#endif // dbshadow_process_h_

