#ifndef breg3d_voxel_grid_h
#define breg3d_voxel_grid_h

#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>


class breg3d_voxel_grid
{
public:
  //: Construct a new axis-aligned voxel grid. This constructor does not write any data to disk.
  breg3d_voxel_grid(const char* filename, unsigned nx, unsigned ny, unsigned nz, vgl_point_3d<double> corner, double vox_size);
  //: Initialize the voxel data. Any exisiting data on disk will be overwritten.
  bool init_voxels(float init_prob, float init_mg_means, float init_mg_vars, float init_mg_weights);
  //: Initialize the voxel grid with default values. Any exisiting data on disk will be overwritten.
  bool init_voxels();

  //: Load a slice of voxel probability data from disk.
  inline void load_prob_slice(vil_image_view<float> &data_prob, unsigned k);
  //: Load a slice of mixture of gaussian appearance model data from disk.
  inline void load_mg_slice(vcl_vector<vil_image_view<float> > &data_mg, unsigned k);
  //: Save a slice of voxel probability data to disk.
  inline bool save_prob_slice(vil_image_view<float> &data_prob, unsigned k);
  //: Save a slice of voxel mixture of gaussian appearance model data to disk.
  inline bool save_mg_slice(vcl_vector<vil_image_view<float> > &data_mg, unsigned k);

    
  //: Given an image and a corresponding (correct) camera, update the voxel grid
  bool update_voxels(vil_image_view<float> const& frame, vpgl_camera<double>* cam);
  //: Given an image and a corresponding camera, return the cost
  vil_image_view<float> camera_score(vil_image_view<float> const& frame, vpgl_camera<double>* cam);
  //: Given an image and a corersponding camera, return the probabilities that no voxel produced the observed intensity
  vil_image_view<float> camera_cost(vil_image_view<float> const& frame, vpgl_camera<double>* cam);

  //: Given an image, corresponding camera, and virtual camera, produce a rendered image from the virtual camera
  void virtual_view(vil_image_view<float> const& frame, vpgl_camera<double> const* cam, vpgl_camera<double> const* vcam, 
                    vil_image_view<float> &virtual_frame, vil_image_view<float> &vis, vil_image_view<unsigned char> &height_map);

  //: Given a camera, generate the expected image
  void expected_image(vpgl_camera<double> const* cam, vil_image_view<float> &expected_image, vil_image_view<float> &mask);


private:
  // do not allow default constructor
  breg3d_voxel_grid():std_init_value_(0.1f),min_vox_prob_(1e-5f),max_vox_prob_(1 - 1e-5f){};

  // store data as images on disk
  vcl_vector<vcl_string> prob_image_fnames_;
  // one set of images for each mode in the mixture of gaussians
  vcl_vector<vcl_vector<vcl_string> > mg_image_fnames_;
  

  unsigned nx_;
  unsigned ny_;
  unsigned nz_;

  static const unsigned n_mg_modes_ = 3;
  const vcl_string image_format_;
  
  // 3D location of the corner corresponding to data_prob_[0][0][0]
  vgl_point_3d<double> corner_;
  // 3D vectors corresponding to one voxel step in each direction;
  vgl_vector_3d<double> i_step_;
  vgl_vector_3d<double> j_step_;
  vgl_vector_3d<double> k_step_;

  vcl_vector<vcl_string> temporary_file_filenames(unsigned temp_idx, unsigned nfiles);

  vil_image_view<float> pixel_probabilities(vil_image_view<float> const& backproj_frame, vcl_vector<vil_image_view<float> > const& mg_modes);
  // returns an actual probability value (0 - 1) that voxels produce an intensity in the range (I-delta, I+delta)
  vil_image_view<float> pixel_probabilities_range(vil_image_view<float> const& backproj_frame, vcl_vector<vil_image_view<float> > const& mg_modes, float delta = 0.01);

  // returns an expected image
  vil_image_view<float> expected_intensities(vcl_vector<vil_image_view<float> > const& mg_modes);

  // methods for transforming from image to plane coordinates
  void compute_plane_image_H(vpgl_camera<double> const* cam, unsigned grid_zval, vgl_h_matrix_2d<double> &H_plane_to_image, vgl_h_matrix_2d<double> &H_image_to_plane);

  //: transform a plane using a homography. Note that the inverse homography (mapping dest coordinates to src coordinates) must be passed in.
  void transform_plane(vil_image_view<float> const& input, vgl_h_matrix_2d<double> const& invH, vil_image_view<float>& output, double off_x=0.0, double off_y=0.0);

  vil_image_view<float> update_slice_prob(vil_image_view<float> const &slice_prob_curr, 
                                          vil_image_view<float> const& visX, vil_image_view<float> const& preX, vil_image_view<float> const& postX,
                                          vil_image_view<float> const& pixel_probabillities);

  void update_mg_models(vcl_vector<vil_image_view<float> > & slice_mg, vil_image_view<float> const& slice_prob, vil_image_view<float> const& backproj_frame);


  vcl_string base_dirname_;

  // initialization values
  const float std_init_value_;
  // minimum and maximum allowed voxel probability values
  const float min_vox_prob_;
  const float max_vox_prob_;

};


#endif
