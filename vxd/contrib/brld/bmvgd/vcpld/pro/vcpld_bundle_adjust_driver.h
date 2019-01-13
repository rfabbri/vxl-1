// This is vcpld_bundle_adjust_driver.h
#ifndef vcpld_bundle_adjust_driver_h
#define vcpld_bundle_adjust_driver_h
//:
//\file
//\brief Class to run vcpld_*bundle_adjust using a 3D curve sketch
//\author Ricardo Fabbri (rfabbri), Brown University  (@gmail.com)
//\date Fri Nov 13 18:03:26 EST 2009
//

#include <vcpld/pro/vcpld_bundle_adjust_sink.h>
#include <bmcsd/bmcsd_curve_3d_sketch.h>
#include <vcpld/algo/vcpld_distmap_bundle_adjust.h>
#include <bmcsd/algo/bmcsd_data.h>

class vcpld_bundle_adjust_driver {
public: 

  // The caller is responsible for managing csk memory, deleting csk only after
  // this class has been used.
  // \param[in] optimize_one_view: optimizes every view independently, without
  // optimizing the 3D structure.
  vcpld_bundle_adjust_driver(
      const bmcsd_curve_stereo_data_path &dpath, 
      bmcsd_curve_3d_sketch *csk,
      const std::set<unsigned> &viewset,
      bool optimize_one_view,
      bool curve_ransac
      )
    :
      dpath_(dpath),
      csk_(csk), 
      viewset_(viewset),
      optimize_one_view_(optimize_one_view),
      curve_ransac_(curve_ransac),
      initialized_(false)
  { }

  ~vcpld_bundle_adjust_driver() {}

  //: Initializes the processing, e.g. setting up a processing graph, computing
  // loading frame data, etc.
  bool init();

  //: Runs all instances of the 2-view matching and reconstruction
  bool run(unsigned long timestamp=1);

  const std::vector< vpgl_perspective_camera<double> > &optimized_cameras()
  { return *optimized_cam_; }

  unsigned nviews() const { return nviews_; }

  const std::vector<unsigned> &views() const { return views_; }

protected:
  //: Inputs
  bmcsd_curve_stereo_data_path dpath_;
  const bmcsd_curve_3d_sketch *csk_;

  //: Outputs
  std::vector< vpgl_perspective_camera<double> > *optimized_cam_;
  typedef std::vector<vgl_point_3d<double> > curve_points;
  std::vector< curve_points > *optimized_pts3d_;

  unsigned nviews_;
  //: the views that were selected to be optimized
  std::vector<unsigned> views_;

  //: the superset of views the user requested to be optimized
  std::set<unsigned> viewset_;

  //: true if each view's 6DOF is to be optimized independently
  bool optimize_one_view_;

  //: if optimize_one_view_ is true, this does random subset selection of curves
  // during optimization.
  bool curve_ransac_;

  //: true if init has been called
  bool initialized_;
  //: pool of processors.
  std::vector<bprod_process_sptr> cam_src_;
  std::vector<bprod_process_sptr> edg_src_;
  std::vector<bprod_process_sptr> edg_dt_;
  bprod_process_sptr curve_bundler_;

  bool get_views_to_optimize(const std::set<unsigned> &viewset, 
      std::vector<unsigned> *views_ptr);
  void get_curve_visibility_using_reference_views(
      std::vector<std::vector<bool> > *mask_ptr );
};

#endif // vcpld_bundle_adjust_driver_h
