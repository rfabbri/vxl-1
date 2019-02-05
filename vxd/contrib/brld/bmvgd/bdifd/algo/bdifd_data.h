// This is bdifd_data.h
#ifndef bdifd_data_h
#define bdifd_data_h
//:
//\file
//\brief Functions to create multiview datasets 
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri.github.io)
//\date Sat May 13 18:41:53 EDT 2006
//

#include <bdifd/bdifd_camera.h>
#include <vsol/vsol_line_2d_sptr.h>

class bdifd_rig;

//: Defines some multiview differential-geometric synthetic data and utilities
class bdifd_data {
  public:

  static void
  project_into_cams(
      const std::vector<bdifd_3rd_order_point_3d> &crv3d, 
      const std::vector<bdifd_camera> &cam,
      std::vector<std::vector<bdifd_3rd_order_point_2d> > &xi //:< image coordinates
      );

  static void 
  project_into_cams(
      const std::vector<bdifd_3rd_order_point_3d> &crv3d, 
      const std::vector<bdifd_camera> &cam,
      std::vector<std::vector<vsol_point_2d_sptr> > &xi //:< image coordinates
      );

  static void 
  project_into_cams_without_epitangency(
      const std::vector<std::vector<bdifd_3rd_order_point_3d> > &crv3d,
      const std::vector<bdifd_camera> &cam,
      std::vector<std::vector<bdifd_3rd_order_point_2d> > &crv2d_gt,
      double epipolar_angle_thresh);

  static void
  space_curves_ctspheres( std::vector<std::vector<bdifd_3rd_order_point_3d> > &crv3d );

  static void
  space_curves_olympus_turntable( std::vector<std::vector<bdifd_3rd_order_point_3d> > &crv3d);

  static void 
  space_curves_digicam_turntable_sandbox( std::vector<std::vector<bdifd_3rd_order_point_3d> > &crv3d);

  static void space_curves_digicam_turntable_medium_sized(
    std::vector<std::vector<bdifd_3rd_order_point_3d> > &crv3d
    );

  static void 
  space_curves_ctspheres_old( std::vector<std::vector<bdifd_3rd_order_point_3d> > &crv3d );

  static void 
  project_into_cams(
      const std::vector<std::vector<bdifd_3rd_order_point_3d> > &crv3d,
      const std::vector<bdifd_camera> &cam,
      std::vector<std::vector<bdifd_3rd_order_point_2d> > &crv2d_gt);

  static void 
  max_err_reproj_perturb(
      const std::vector<std::vector<bdifd_3rd_order_point_2d> > &crv2d_gt_,
      const std::vector<bdifd_camera> &cam_,
      const bdifd_rig &rig,
      double &err_pos,
      double &err_t,
      double &err_k,
      double &err_kdot,
      unsigned &i_pos, 
      unsigned &i_t, 
      unsigned &i_k, 
      unsigned &i_kdot,
      unsigned &nvalid
      );

  static void 
  err_reproj_perturb(
      const std::vector<std::vector<bdifd_3rd_order_point_2d> > &crv2d_gt_,
      const std::vector<bdifd_camera> &cam_,
      const bdifd_rig &rig,
      std::vector<double> &err_pos_sq,
      std::vector<double> &err_t,
      std::vector<double> &err_k,
      std::vector<double> &err_kdot,
      std::vector<unsigned> &valid_idx
      );

  //---------------------------------------------------------------------------
  static void get_lines(
      std::vector<vsol_line_2d_sptr> &lines,
      const std::vector<bdifd_3rd_order_point_2d> &C_subpixel,
      bool do_perturb=false,
      double pert_pos=0.0,
      double pert_tan=0.0
      );

  static void 
  get_circle_edgels(
      double radius, 
      std::vector<vsol_line_2d_sptr> &lines,
      std::vector<bdifd_3rd_order_point_2d> &C_subpixel,
      bool do_perturb=false,
      double pert_pos=0.1,
      double pert_tan=10
      );

  static void get_ellipse_edgels(
      double ra, 
      double rb, 
      std::vector<vsol_line_2d_sptr> &lines,
      std::vector<bdifd_3rd_order_point_2d> &C_subpixel,
      bool do_perturb,
      double pert_pos,
      double pert_tan
      );

  static vgl_point_3d<double> 
  get_point_crv3d(const std::vector<std::vector<bdifd_3rd_order_point_3d> > &crv3d, unsigned i);

  //---------------------------------------------------------------------------
  // Utilities to output traditional point dataset (no differential geometry)

  static void 
  get_digital_camera_point_dataset(
      std::vector<vpgl_perspective_camera<double> > *pcams, 
      std::vector<std::vector<vgl_point_2d<double> > > *pimage_pts, 
      std::vector<vgl_point_3d<double> > *pworld_pts, 
      const std::vector<double> &view_angles);
};

//: Class dealing with a turntable camera configuration.
class bdifd_turntable {
public:
  //- function to set the turntable params

  //- create_camera(frame_idx, internal matrix)

  //- function to return/set params for ctspheres
  //- function to run/set params for olympus's turntable

  static vpgl_perspective_camera<double> *
  camera_ctspheres(
      unsigned frm_index,
      const vpgl_calibration_matrix<double> &K);

  static void 
  internal_calib_ctspheres(vnl_double_3x3 &m, double x_max_scaled=4000.0);

  static void 
  internal_calib_olympus(vnl_double_3x3 &m, double x_max_scaled=0, unsigned  crop_x=0, unsigned  crop_y=0);

  static vpgl_perspective_camera<double> * 
  camera_olympus(
      double theta,
      const vpgl_calibration_matrix<double> &K);

  // samples turtable center but on a spherical configurations of cameras
  // with cameras poiting to center
  static void cameras_olympus_spherical(
      std::vector<vpgl_perspective_camera<double> > *pcams,
      const vpgl_calibration_matrix<double> &K,
      bool enforce_minimum_separation=false,
      bool perturb=false);
};


#endif // bdifd_data_h

