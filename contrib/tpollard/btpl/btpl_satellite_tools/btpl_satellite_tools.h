#ifndef btpl_satellite_tools_h_
#define btpl_satellite_tools_h_

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_proj_camera.h>


// Various operations for managing a large database of satellite images.  Currently
// all this code is specific to the baghdad database and should be generalized
// some day.


class btpl_satellite_tools {

public:

  btpl_satellite_tools();

  // Draw the density of images intersecting the scene on a lidar reference image.
  void compute_coverage();

  // Plot the range of lighting and viewpoint directions.
  void plot_directions();


  // SCENE SPECIFIC FUNCTIONS:---------------------------------
  void set_scene(
    vcl_string scene_name );

  // STEP 1: Make a list of all images containing the scene.
  void get_coverage_list();

  // STEP 2: Create refined cameras around the scene. 
  void refine_rational_cameras();

  // STEP 3: Get subimages and cameras.
  void get_subimages();

  // STEP 4: Put lighting directions into a file.
  void get_lighting_list();

  // STEP 5 NO LONGER NEEDED:
  // STEP 5a: Project the image calibration points into each image and
  // record the observations.
  void get_img_cal_obs();
  // STEP 5b: Normalize the images.
  void normalize_images();

  // Get the scene bounds in the new coordinate system.
  void scene_bounds();

  // Convert a world point in lat/lon into the local reference frame
  // determined by the current scene.
  vgl_point_3d<double> transform_wp(
    vgl_point_3d<double> wp );


protected:

  // Parameters related to the scene, set in set_scene.
  vcl_string raw_dir_, output_dir_, lidar_ref_img_, coverage_img_, directions_file_, 
    scene_desc_, camera_cal_pts_, img_cal_pts_, test_pts_, img_coverage_list_, img_cal_obs_list_,
    subimg_dir_, subimg_cameras_, subimg_lights_, ref_subimg_dir_;
  int lidar_ref_ni_, lidar_ref_nj_, img_cal_ref_img_;
  vgl_point_3d<double> scene_coord_;
  vgl_box_3d<double> subimg_vol_;

  // Get all images/cameras/imd files for the scene.
  void filenames_for_scene(
    vcl_vector< vcl_string >* img_names,
    vcl_vector< vcl_string >* cam_names,
    vcl_vector< vcl_string >* imd_names );

  // Pull out a sub image from a large satellite image with a projective
  // camera good in that region.
  void get_projective_subimg(
    vcl_string img_file,
    vcl_string cam_file,
    const vgl_box_3d<double>& vol,
    vil_image_view<unsigned short>& subimg,
    vpgl_proj_camera<double>& subimg_cam );
};


#endif // btpl_satellite_tools_h_
