#ifndef _bvaml_world_h_
#define _bvaml_world_h_

//:
// \file
// \brief World
// \author Thomas Pollard
// \date 012/21/06
// 
//   This is....

#include "bvaml_params.h"
#include "bvaml_supervoxel.h"

#include <vcl_vector.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_proj_camera.h>


class bvaml_world {

  friend class bvaml_world_viewer;

public:

  // Create a world in a directory containing a param.txt file.
  bvaml_world(
    bvaml_params* params );

  // Process the given image, detecting changes and training the voxel
  // model on it if update_world == true. Detected changes are stored in 
  // change_file as a probability map image with 0=0, 255=1.
  void process(
    int mode,
    vcl_string namebase,
    const vil_image_view<vxl_byte>& img,
    const vpgl_proj_camera<double>& camera,
    const vnl_vector<float>& light,
    const vcl_vector< vgl_point_2d<int> >& inspect_pixels );

  // Process the job in the param file.
  void process_job();

  // Improve the estimates of surface voxels by incorporating a MRF prior.
  void refine_surface();
  void refine_surface2();

  // Interpolate appearances a1 and a2 to predict appearance a3.
  void predict_appearance(
    int a1, int a2, int a3 );

  // Set the planes specified in the file to have probability 1 and 
  // all voxels above and below to have prob 0.
  void set_ground_plane(
    vcl_string ground_file );



protected:

  bvaml_params* params_;

  vcl_vector< bvaml_supervoxel > supervoxels_;
  vil_image_view<vxl_byte> last_norm_img_;
  
  // Load the voxel for the index, with the data for the specified
  // fields loaded.
  void get_voxel(
    const vgl_point_3d<int>& voxel_index,
    bvaml_voxel& v,
    bool load_occupancy,
    bool load_misc,
    bool load_appearance,
    const vnl_vector<float>* local_light );

  // Storage for supervoxels and indexing function.
  int supervoxel_index(
    const vgl_point_3d<int>& p ){
      return p.z() + params_->num_supervoxels.z()*p.y() +
        params_->num_supervoxels.z()*params_->num_supervoxels.y()*p.x();
    }

  // Find the supervoxel containing voxel_index and the index withing
  // this supervoxel.
  bool get_voxel_location(
    const vgl_point_3d<int>& voxel_index,
    vgl_point_3d<int>& supervoxel,
    vgl_point_3d<int>& voxel_in_supervoxel );


  // Voxel index to world coordinate conversion.
 void get_voxel_index(
    const vgl_point_3d<float>& p,
    vgl_point_3d<int>& voxel_index );
 void get_world_coord(
    const vgl_point_3d<int>& voxel_index,
    vgl_point_3d<float>& p );

 void occupancy_to_heights( unsigned m );
 void heights_to_occupancy( unsigned m );

};


#endif // _bvaml_world_h_
