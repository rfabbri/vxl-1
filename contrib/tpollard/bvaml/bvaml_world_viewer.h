#ifndef _bvaml_world_viewer_h_
#define _bvaml_world_viewer_h_

//:
// \file
// \brief World
// \author Thomas Pollard
// \date 012/21/06
// 
//   This is....

#include "bvaml_params.h"
#include "bvaml_world.h"
#include "bvaml_ray.h"


class bvaml_world_viewer {

public:

  bvaml_world_viewer(
    bvaml_params* params ) :
      params_( params ){}

  // Write a .raw file for use with drishti.
  void write_raw_world(
    bvaml_world* world,
    const vcl_string& filename );

  // Write the model to a x3d file with voxels with occupancy probability > 
  // threshold being displayed and 0 < scale <= 1 being the reduction of voxels.
  void write_x3d_world(
    bvaml_world* world,
    const vcl_string& filename,
    float threshold,
    float scale );
/*
  // Write the model as a stack of images to the given directory.
  void write_world(
    bvaml_world* world,
    const vcl_string& slice_dir,
    float scale );
*/
  // Add rays to be viewed in an x3d inspection file.
  // CURRENTLY INACTIVE
  void start_inspection_file();
  void add_inspection_ray(
    const vcl_vector< vgl_point_3d<int> >& voxel_indices,
    const bvaml_ray& ray,
    bool before_update );
  void save_inspection_file(
    const vcl_string& filename );

protected:

  void draw_world_axis(
    vcl_ostream& stream );

  void get_face_points(
    const vgl_point_3d<float>& center,
    const vgl_vector_3d<float>& normal,
    vgl_point_3d<float>& p1,
    vgl_point_3d<float>& p2, 
    vgl_point_3d<float>& p3, 
    vgl_point_3d<float>& p4 );

  vcl_stringstream inspection_file_;

  bvaml_params* params_;

};


#endif // _bvaml_world_viewer_h_
