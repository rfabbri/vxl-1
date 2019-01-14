// This is mw_synthetic_corresp_process.h
#ifndef mw_synthetic_corresp_process_h
#define mw_synthetic_corresp_process_h
//:
//\file
//\brief Process to generate synthetic correspondence data
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date 01/11/07 03:44:52 EST
//

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>
#include <vgl/vgl_fwd.h>
#include <dbdet/dbdet_keypoint_corr3d_sptr.h>


template <class T> 
class vpgl_perspective_camera;

template <class T> 
class vpgl_calibration_matrix;


struct bdifd_3rd_order_point_2d;

//: Process for generating synthetic correspondence data and calibration for testing struction and
// motion recovery algorithms
class mw_synthetic_corresp_process : public bpro1_process {

public:

  mw_synthetic_corresp_process();
  virtual ~mw_synthetic_corresp_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name();

  int input_frames();
  int output_frames();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

protected:

  std::vector<vpgl_perspective_camera<double> *> pcam_;
  std::vector<vgl_point_3d<double> > vgl_pts_;
  std::vector<vpgl_perspective_camera<double> *> pcam_ini_;
  std::vector<vgl_point_3d<double> > vgl_pts_ini_;
  std::vector<unsigned> pt_id_;
  std::vector<std::vector<bdifd_3rd_order_point_2d> > crv2d_;
  unsigned nviews_;

  void define_dataset(vpgl_calibration_matrix<double> **K, unsigned &ncols, unsigned &nrows);
  void  generate_corr_pts(std::vector<dbdet_keypoint_corr3d_sptr> &corr_pts);
  void initialize_poses(
    const vpgl_calibration_matrix<double> &K, 
    std::vector<vpgl_perspective_camera<double> *> &pcam_ini,
    std::vector<vgl_point_3d<double> > &vgl_pts_ini
    ) const;
};


#endif // mw_synthetic_corresp_process_h

