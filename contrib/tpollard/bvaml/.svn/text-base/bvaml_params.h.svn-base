#ifndef _bvaml_params_h_
#define _bvaml_params_h_

//:
// \file
// \brief Parameters
// \author Thomas Pollard
// \date 12/20/06
// 
//   A class for holding both parameters for the voxel world and 
// jobs to be run on the world.  Later these two kinds of parameters
// may be separated into separate classes.
//
//   These parameters will in general be read from one or more 
// parameter files with following format:
//
/*

bvaml parameter file
# This must be the first line.  Lines starting with # will be ignored.


# MODEL PARAMETERS
# All model parameters have default values if not given.

model_dir "model directory"
# Directory where the model is stored.

output_dir "output directory"
# Directory for saving all results.

corner "cornerX" "cornerY" "cornerZ"
# The bottom corner of the modeled voxel volume.

voxel_length "voxel edge length"
# The dimensions of a single voxel.

supervoxel_length "supervoxel length"
# Number of voxels per edge of a supervoxel.

num_supervoxels "nsvX" "nsvY" "nsvZ"
# Number of supervoxels per dimension of the volume.

appearance_model "appearance index"
# The index for which appearance model to use. See indices far below.

num_mixtures "number of mixtures"
# Number of mixuture modes in the MoG model.


# JOB PARAMETERS
# All "light file" parameters will be ignored and should be entered as "NONE" if
# the appearance model doesn't use light information.  Any "sequence subset" of a 
# job sequence can be entered as in matlab, ex 1:2:20

training_images "image directory" "camera file" "light file" "sequence subset"
# Train the model on all images in the directory, or every nth image where 
# n is the optional frame increment parameter.  Changes will be detected and
# saved.

detection_images "image directory" "camera file" "light file" "sequence subset"
# Same as training only model will not be updated.

render_images "original image directory"(only for img size) "camera file" "light file" "sequence subset"
# Render new views of the scene from the cameras provided.

render_from_new_view "image directory" "camera file" "new camera file" "sequence subset"
# project images onto the world and then render from a new view.

save_normalized_images "image directory" "camera file" "light file" "sequence subset"
# normalize each image with current model.

generate_prob_image "image file" "camera file"

predict_appearance a1 a2 a3
# Predict the appearance of mode a3 using linear interpolation on a1, a2.

write_x3d_world "occupancy threshold" "scene scaling"
# Write a x3d file of the world after all other jobs.

write_raw_world
# Write a .raw file of the world for use with drishti.

set_ground_planes "ground_plane_file"
# Sets the ground planes to prob 1.


image_scale "image scale"
# The factor all input images should be scaled down by before using. This should
# eventually be calculated automatically.

normalize_intensities "TRUE/false"
# Normalize image intensities before training.

order_by_date "TRUE/false"
# Process images by date, assuming Baghdad dating system, can be generalized.
# This must appear before any image process in the param file.

inspect_frame "frame number" "pixel1x" pixel1y" "pixel2x" "pixel2y" ...
# Produce an x3d visualization of the voxels projecting into the given pixels
# the given frame.

inspect_point "point1x" "point1y" "point1z" "point2x" ...
# Print to screen information about the voxels at the given points at every
# image.

*/

#include <vcl_string.h>
#include <vcl_cmath.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vul/vul_awk.h>


class bvaml_params {

public:

  // Only constructor with default params.
  bvaml_params() :
    model_dir( "NO MODEL DIRECTORY" ),
    output_dir( "F:/testing" ),
    ground_plane_file( "NONE" ),
    image_scale( (float)1.0 ),
    normalize_intensities( false ),
    order_by_date( false ),
    corner( vgl_point_3d<double>( 0.0, 0.0, 0.0 ) ),
    voxel_length( (float).1 ),
    supervoxel_length( 50 ),
    num_supervoxels( vgl_vector_3d<int>( 2, 2, 2 ) ),
    appearance_model( 0 ),
    num_mixtures( 3 ),
    initial_std( (float)0.2 ),
    empty_thresh( (float)0.0 ),
    misc_storage_size( 4 ),
    min_prob_to_allow_empty_voxels( (float).6 ),
    write_raw_world( false ){}

  // Read the params from a file.
  bool read_params( vcl_string param_file );

  // Print all parameters to screen.
  void print_summary();


  // JOB PARAMETERS:--------------------------------------------
  
  // All image files to be processed.
  vcl_vector<vcl_string> images;

  // Cameras for the images.
  vcl_vector< vpgl_proj_camera<double> > cameras;

  // Special camera for rendering registered images.
  vpgl_proj_camera<double> render_camera;

  // Lights for the images.
  vcl_vector< vnl_vector<float> > lights;

  // Determines how each image/camera pair will be processed:
  // 0 for training
  // 1 for detecting change
  // 2 for rendering new view
  // 3 for projecting an image onto the world
  // 4 for rendering a new view from a projected image.
  // 5 for creating a probability map in baml_world.prob_img
  // 6 for normalizing images.
  vcl_vector<int> process_modes;

  // These pixels in each image will be inspected during training
  // and voxels output to vrml.
  vcl_vector< vcl_vector< vgl_point_2d<int> > > inspect_pixels;

  // These voxels' appearance models will be printed during training.
  vcl_vector< vgl_point_3d<int> > inspect_voxels;

  // Directory storing the model files.
  vcl_string model_dir;

  // Directory for storing all output.
  vcl_string output_dir;

  // This should be the scale factor to reduce the image to approximately
  // voxel sized pixels.  Will eventually be computed automatically.
  float image_scale;

  // Normalize all images before training.
  bool normalize_intensities;

  // Train the image in time order.
  bool order_by_date;

  // Write a x3d file of the world after the last training img.  First 
  // parameter is occupancy threshold, second is scaling.
  vcl_vector<float> write_x3d_world;

  // List of prediction jobs to be done.
  vcl_vector< vnl_vector<int> > prediction_jobs;

  bool write_raw_world;

  vcl_string ground_plane_file;


  // GEOMETRY PARAMETERS:---------------------------------------

  // The world will be a rectangular volume starting at given corner.
  vgl_point_3d<double> corner;

  // The length of one edge of the voxels filling the world volume.
  double voxel_length;

  // Voxels are stored in larger supervoxels with the following
  // number of voxels per side.
  int supervoxel_length;

  // Number of supervoxels filling the scene in each direction.
  vgl_vector_3d<int> num_supervoxels;

  // A voxel whose occupany prob is less than this will be ignored.
  float empty_thresh;

  // If a pixel has probability less that this, all voxels on the ray will
  // have their occupancy probabilities raised to above the empty_thresh.
  float min_prob_to_allow_empty_voxels;


  // APPEARANCE PARAMETERS:---------------------------------

  // Type of appearance model estimated at each voxel.
  // 0 = mixture of gaussians color model
  // 1 = multi mixture of gaussians
  // 2 = physical
  // 3 = RBF
  int appearance_model;

  // Number of nodes in the Gaussian mixture
  int num_mixtures;
  
  // Parameters for the mixture_model.
  float initial_std; //UNUSED.

  // Number of floats per voxel for misc storage.
  int misc_storage_size;


  // DERIVED PARAMETERS:---------------------------------

  // Numbers of voxels in each direction.
  vgl_vector_3d<int> num_voxels(){ 
    return supervoxel_length*num_supervoxels; }

  // The occupancy probability of each voxel when the world is initialized.
  float initial_occupancy_prob(){
    double mean_num_voxels = 
      ( num_voxels().x()+num_voxels().y()+num_voxels().z() )/3.0;
    return (float)(1.0/mean_num_voxels);
  }
/*
  // Size of a single mixture component in floats.
  int data_per_mixture(){
    if( appearance_model == 0 )
      return 3;
    else if( appearance_model == 1 )
      return 5;
    else if( appearance_model == 2 )
      return 10; 
    return -1; }
    

  // The size of a voxel's data in floats.
  int appearance_model_size(){
    return data_per_mixture()*num_mixtures; }
*/

protected:

  bool get_cameras(
    vcl_string camera_file,
    vcl_vector< vpgl_proj_camera<double> >& cameras );

  bool get_images(
    vcl_string image_dir,
    vcl_vector< vcl_string >& images );

  bool get_lights(
    vcl_string light_file,
    vcl_vector< vnl_vector<float> >& lights );

  // Parse a range string of form like 10:5:45
  bool get_range(
    vcl_string range_str,
    vnl_vector_fixed<int,3>& range_indices );

  void reorder_by_date(
    const vcl_vector< vcl_string >& file_list,
    vcl_vector< unsigned >& new_order );
};


#endif // _bvaml_params_h_
